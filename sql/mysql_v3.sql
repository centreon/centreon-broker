-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--         Performance data          --
-- ------------------------------------

-- rt_acknowledgements
-- rt_customvariables
-- log_data_bin
-- rt_downtimes
-- rt_eventhandlers
-- rt_flappingstatuses
-- rt_hosts_hosts_dependencies
-- rt_hosts_hosts_parents
-- rt_hoststateevents
-- rt_index_data
-- rt_issues
-- rt_issues_issues_parents
-- rt_metrics
-- rt_modules
-- rt_notifications
-- rt_schemaversion
-- rt_services_services_dependencies
-- rt_servicestateevents


--
-- Holds the current version of the database schema.
--
CREATE TABLE rt_schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
) ENGINE=InnoDB;
INSERT INTO rt_schemaversion (software, version) VALUES ('centreon-broker', 3);


--
-- Hosts dependencies.
--
CREATE TABLE rt_hosts_hosts_dependencies (
  dependent_host_id int NOT NULL,
  host_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent boolean default NULL,
  notification_failure_options varchar(15) default NULL,

  FOREIGN KEY (dependent_host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Hosts parenting relationships.
--
CREATE TABLE rt_hosts_hosts_parents (
  child_id int NOT NULL,
  parent_id int NOT NULL,

  UNIQUE (child_id, parent_id),
  FOREIGN KEY (child_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Services dependencies.
--
CREATE TABLE rt_services_services_dependencies (
  dependent_host_id int NOT NULL,
  dependent_service_id int NOT NULL,
  host_id int NOT NULL,
  service_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent boolean default NULL,
  notification_failure_options varchar(15) default NULL,

  FOREIGN KEY (dependent_host_id, dependent_service_id) REFERENCES rt_services (host_id, service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id, service_id) REFERENCES rt_services (host_id, service_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Holds acknowledgedments information.
--
CREATE TABLE rt_acknowledgements (
  acknowledgement_id int NOT NULL auto_increment,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  author varchar(64) default NULL,
  comment_data varchar(255) default NULL,
  deletion_time int default NULL,
  instance_id int default NULL,
  notify_contacts boolean default NULL,
  persistent_comment boolean default NULL,
  state smallint default NULL,
  sticky boolean default NULL,
  type smallint default NULL,

  PRIMARY KEY (acknowledgement_id),
  UNIQUE (entry_time, host_id, service_id),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES rt_instances (instance_id)
    ON DELETE SET NULL
) ENGINE=InnoDB;


--
-- Custom variables.
--
CREATE TABLE rt_customvariables (
  customvariable_id int NOT NULL auto_increment,
  host_id int default NULL,
  name varchar(255) default NULL,
  service_id int default NULL,

  default_value varchar(255) default NULL,
  modified boolean default NULL,
  type smallint default NULL,
  update_time int default NULL,
  value varchar(255) default NULL,

  PRIMARY KEY (customvariable_id),
  UNIQUE (host_id, name, service_id)
) ENGINE=InnoDB;


--
-- Downtimes.
--
CREATE TABLE rt_downtimes (
  downtime_id int NOT NULL auto_increment,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  actual_end_time int default NULL,
  actual_start_time int default NULL,
  author varchar(64) default NULL,
  cancelled boolean default NULL,
  comment_data text default NULL,
  deletion_time int default NULL,
  duration int default NULL,
  end_time int default NULL,
  fixed boolean default NULL,
  instance_id int default NULL,
  internal_id int default NULL,
  start_time int default NULL,
  started boolean default NULL,
  triggered_by int default NULL,
  type smallint default NULL,
  is_recurring boolean default NULL,
  recurring_interval int default NULL,
  recurring_timeperiod varchar(200) default NULL,

  PRIMARY KEY (downtime_id),
  UNIQUE (entry_time, host_id, service_id),
  UNIQUE (entry_time, host_id, internal_id),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES rt_instances (instance_id)
    ON DELETE SET NULL
) ENGINE=InnoDB;


--
-- Event handlers.
--
CREATE TABLE rt_eventhandlers (
  host_id int default NULL,
  service_id int default NULL,
  start_time int default NULL,

  command_args varchar(255) default NULL,
  command_line varchar(255) default NULL,
  early_timeout boolean default NULL,
  end_time int default NULL,
  execution_time double default NULL,
  output varchar(255) default NULL,
  return_code smallint default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  timeout smallint default NULL,
  type smallint default NULL,

  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Historization of flapping statuses.
--
CREATE TABLE rt_flappingstatuses (
  flappingstatus_id int NOT NULL auto_increment,
  host_id int default NULL,
  service_id int default NULL,
  event_time int default NULL,

  comment_time int default NULL,
  event_type smallint default NULL,
  high_threshold double default NULL,
  internal_comment_id int default NULL,
  low_threshold double default NULL,
  percent_state_change double default NULL,
  reason_type smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (flappingstatus_id),
  UNIQUE (host_id, service_id, event_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Correlated issues.
--
CREATE TABLE rt_issues (
  issue_id int NOT NULL auto_increment,
  host_id int default NULL,
  service_id int default NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,

  PRIMARY KEY (issue_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Issues parenting.
--
CREATE TABLE rt_issues_issues_parents (
  child_id int NOT NULL,
  end_time int default NULL,
  start_time int NOT NULL,
  parent_id int NOT NULL,

  FOREIGN KEY (child_id) REFERENCES rt_issues (issue_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_id) REFERENCES rt_issues (issue_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Nagios modules.
--
CREATE TABLE rt_modules (
  module_id int NOT NULL auto_increment,
  instance_id int NOT NULL,

  args varchar(255) default NULL,
  filename varchar(255) default NULL,
  loaded boolean default NULL,
  should_be_loaded boolean default NULL,

  PRIMARY KEY (module_id),
  FOREIGN KEY (instance_id) REFERENCES rt_instances (instance_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
--  Notifications.
--
CREATE TABLE rt_notifications (
  notification_id int NOT NULL auto_increment,
  host_id int default NULL,
  service_id int default NULL,
  start_time int default NULL,

  ack_author varchar(255) default NULL,
  ack_data text default NULL,
  command_name varchar(255) default NULL,
  contact_name varchar(255) default NULL,
  contacts_notified boolean default NULL,
  end_time int default NULL,
  escalated boolean default NULL,
  output text default NULL,
  reason_type int default NULL,
  state int default NULL,
  type int default NULL,

  PRIMARY KEY (notification_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
--  Host states.
--
CREATE TABLE rt_hoststateevents (
  host_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime boolean default NULL,
  state int default NULL,

  UNIQUE (host_id, start_time)
) ENGINE=InnoDB;


--
--  Service states.
--
CREATE TABLE rt_servicestateevents (
  host_id int NOT NULL,
  service_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime boolean default NULL,
  state int default NULL,

  UNIQUE (host_id, service_id, start_time)
) ENGINE=InnoDB;


--
--  Base performance data index.
--
CREATE TABLE rt_index_data (
  index_id int NOT NULL auto_increment,
  host_id int NOT NULL,
  service_id int default NULL,

  check_interval int default NULL,
  hidden boolean NOT NULL default 0,
  host_name varchar(255) default NULL,
  locked boolean NOT NULL default 0,
  must_be_rebuild tinyint NOT NULL default 0,
  rrd_retention int default NULL,
  service_description varchar(255) default NULL,
  special boolean NOT NULL default 0,
  storage_type tinyint NOT NULL default 2,
  to_delete boolean NOT NULL default 0,
  trashed boolean NOT NULL default 0,

  PRIMARY KEY (index_id),
  UNIQUE (host_id, service_id),
  INDEX (host_id),
  INDEX (host_name),
  INDEX (must_be_rebuild),
  INDEX (service_description),
  INDEX (service_id),
  INDEX (trashed)
) ENGINE=InnoDB;

--
--  Metrics.
--
CREATE TABLE rt_metrics (
  metric_id int NOT NULL auto_increment,
  index_id int NOT NULL,
  metric_name varchar(255) NOT NULL,

  crit double default NULL,
  crit_low double default NULL,
  crit_threshold_mode boolean default NULL,
  current_value double default NULL,
  data_source_type tinyint NOT NULL default 0,
  hidden boolean NOT NULL default 0,
  locked boolean NOT NULL default 0,
  min double default NULL,
  max double default NULL,
  to_delete boolean NOT NULL default 0,
  unit_name varchar(32) default NULL,
  warn double default NULL,
  warn_low double default NULL,
  warn_threshold_mode boolean default NULL,

  PRIMARY KEY (metric_id),
  UNIQUE KEY (index_id, metric_name),
  FOREIGN KEY (index_id) REFERENCES rt_index_data (index_id)
    ON DELETE CASCADE,
  INDEX (index_id)
) ENGINE=InnoDB;

--
--  Performance data.
--
CREATE TABLE log_data_bin (
  metric_id int NOT NULL,
  ctime int NOT NULL,
  status tinyint NOT NULL default 3,
  value float default NULL,

  FOREIGN KEY (metric_id) REFERENCES rt_metrics (metric_id)
    ON DELETE CASCADE,
  INDEX (metric_id)
) ENGINE=InnoDB;
