##############
Best Practices
##############

This page references the best practices regarding Centreon Broker
configuration.

*******
General
*******

Set parameters in the *General* tab as follow.

  * log_timestamp = yes
  * log_thread_id = no
  * event_queue_max_size = 50000

***********
Correlation
***********

Not configured at all.

*****
Input
*****

The **pollers** should not have any input configured. Each **central**
(either *central-broker-master* or *central-rrd-master*) daemon should
have one input configured as follow.

  * type = tcp
  * name = central-broker-master / central-rrd-master
  * port = 5669 (central-broker-master) / 5670 (central-rrd-master)
  * host = *empty*
  * failover = *empty*
  * retry_interval = 60
  * buffering_timeout = 0
  * protocol = BBDO
  * tls = auto
  * negociation = yes
  * one_peer_retention_mode = no
  * filter category = *empty* (central-broker-master) / storage (central-rrd-master)
  * compression = auto

******
Logger
******

At least one logger should be configured for each Centreon Broker
configuration file.

  * type = file
  * name = /var/log/centreon-broker/central-broker-master.log (or /var/log/centreon-broker/central-rrd-master.log or something alike)
  * config = yes
  * debug = no
  * error = yes
  * info = yes
  * level = low (Base)
  * max_size = 50000000000

External log file rotation should be provided. A **logrotate**
configuration file is provided along Centreon Broker.

******
Output
******

Poller
======

Pollers should output to the Centreon central.

  * type = tcp
  * name = central
  * port = 5669
  * host = *FQDN or IP address of the central*
  * failover = central-failover
  * retry_interval = 60
  * buffering_timeout = 0
  * protocol = BBDO
  * tls = auto
  * negociation = yes
  * one_peer_retention_mode = no
  * filter category = *empty*
  * compression = auto

The TCP output should have a failover file.

  * type = file
  * name = central-failover
  * path = /var/lib/centreon-broker/central-failover.dat
  * failover = *empty*
  * protocol = BBDO
  * buffering_timeout = 0
  * max_file_size = 500000000
  * filter category = *empty*
  * compression = yes
  * compression buffer size = 2000

Central Broker
==============

The Central Broker is responsible for writing both real-time monitoring
and performance data to the database. Each of these two outputs should
have a failover file.

  * type = sql
  * name = central-broker-sql-master
  * db_type = mysql
  * failover = central-broker-sql-master-failover
  * retry_interval = 60
  * buffering_timeout = 0
  * db_host = localhost
  * db_port = 3306
  * db_user = centreon
  * db_password = centreon
  * db_name = centreon_storage
  * queries_per_transaction = 20000
  * check_replication = no
  * read_timeout = 10
  * filter category = correlation,neb

The SQL output should have a failover file.

  * type = file
  * name = central-broker-sql-master-failover
  * path = /var/lib/centreon-broker/central-broker-sql-master-failover.dat
  * failover = *empty*
  * protocol = BBDO
  * buffering_timeout = 0
  * max_file_size = 500000000
  * filter category = *empty*
  * compression = yes
  * compression buffer size = 2000

This is the performance data output.

  * type = storage
  * name = central-broker-perfdata-master
  * db_type = mysql
  * failover = central-broker-perfdata-master-failover
  * retry_interval = 60
  * buffering_timeout = 0
  * db_host = localhost
  * db_port = 3306
  * db_user = centreon
  * db_password = centreon
  * db_name = centreon_storage
  * queries_per_transaction = 20000
  * read_timeout = 10
  * check_replication = no
  * rebuild_check_interval = 300
  * store_in_data_bin = yes
  * insert_in_index_data = no
  * filter category = neb

The performance data output should have a failover file.

  * type = file
  * name = central-broker-perfdata-master-failover
  * path = /var/lib/centreon-broker/central-broker-perfdata-master-failover.dat
  * failover = *empty*
  * protocol = BBDO
  * buffering_timeout = 0
  * max_file_size = 500000000
  * filter category = *empty*
  * compression = yes
  * compression buffer size = 2000

This is the output to central-rrd.

  * type = tcp
  * name = central-broker-rrd-master
  * port = 5670
  * host = localhost
  * failover = central-broker-rrd-master-failover
  * retry_interval = 60
  * buffering_timeout = 0
  * protocol = BBDO
  * tls = auto
  * negociation = yes
  * one_peer_retention_mode = no
  * filter category = storage
  * compression = auto

The central-rrd output should have a failover file.

  * type = file
  * name = central-broker-rrd-master-failover
  * path = /var/lib/centreon-broker/central-broker-rrd-master-failover.dat
  * failover = *empty*
  * protocol = BBDO
  * buffering_timeout = 0
  * max_file_size = 500000000
  * filter category = *empty*
  * compression = yes
  * compression buffer size = 2000

Central RRD
===========

The Central RRD is responsible for writing RRD files on disk.

  * type = rrd
  * name = central-rrd-master
  * failover = central-rrd-master-failover
  * retry_interval = 60
  * buffering_timeout = 0
  * port = *empty*
  * path = *empty*
  * write_metrics = yes
  * write_status = yes
  * filter category = *empty*

The RRD output should have a failover file.

  * type = file
  * name = central-rrd-master-failover
  * path = /var/lib/centreon-broker/central-rrd-master-failover.dat
  * failover = *empty*
  * protocol = BBDO
  * buffering_timeout = 0
  * max_file_size = 500000000
  * filter category = *empty*
  * compression = yes
  * compression buffer size = 2000

*****
Stats
*****

Statistics should be enabled.

  * type = stats
  * name = poller-stats / central-broker-stats-master / central-rrd-stats-master
  * fifo = /var/lib/centreon-broker/poller-stats.pipe (or /var/lib/centreon-broker-stats-master.pipe or something alike)

*********
Temporary
*********

Temporary must be enabled.

  * type = file
  * name = poller-temporary / central-broker-temporary-master / central-rrd-temporary-master
  * path = /var/lib/centreon-broker/poller-temporary.tmp / /var/lib/centreon-broker/central-broker-master.tmp / /var/lib/centreon-broker/central-rrd-master.tmp
  * protocol = BBDO
  * compression = yes
  * compression buffer size = 2000
  * max_file_size = 500000000
