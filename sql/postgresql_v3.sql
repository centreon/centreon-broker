-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--         Performance data          --
-- ------------------------------------

-- log_data_bin
-- rt_eventhandlers
-- rt_flappingstatuses
-- rt_hoststateevents
-- rt_index_data
-- rt_issues
-- rt_issues_issues_parents
-- rt_metrics
-- rt_notifications
-- rt_schemaversion
-- rt_servicestateevents


--
-- Holds the current version of the database schema.
--
CREATE TABLE rt_schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
);
INSERT INTO rt_schemaversion (software, version) VALUES ('centreon-broker', 3);


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
  execution_time double precision default NULL,
  output varchar(255) default NULL,
  return_code smallint default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  timeout smallint default NULL,
  type smallint default NULL,

  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);


--
-- Historization of flapping statuses.
--
CREATE TABLE rt_flappingstatuses (
  flappingstatus_id serial,
  host_id int default NULL,
  service_id int default NULL,
  event_time int default NULL,

  comment_time int default NULL,
  event_type smallint default NULL,
  high_threshold double precision default NULL,
  internal_comment_id int default NULL,
  low_threshold double precision default NULL,
  percent_state_change double precision default NULL,
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
  issue_id serial,
  host_id int default NULL,
  service_id int default NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,

  PRIMARY KEY (issue_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);


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
);


--
--  Notifications.
--
CREATE TABLE rt_notifications (
  notification_id serial,
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
);


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
);


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
);

--
--  Base performance data index.
--
CREATE TABLE rt_index_data (
  index_id serial,
  host_id int NOT NULL,
  service_id int default NULL,

  check_interval int default NULL,
  hidden boolean NOT NULL default 0,
  host_name varchar(255) default NULL,
  locked boolean NOT NULL default 0,
  must_be_rebuild smallint NOT NULL default 0,
  rrd_retention int default NULL,
  service_description varchar(255) default NULL,
  special boolean NOT NULL default 0,
  storage_type smallint NOT NULL default 2,
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
);

--
--  Metrics.
--
CREATE TABLE rt_metrics (
  metric_id serial,
  index_id int NOT NULL,
  metric_name varchar(255) NOT NULL,

  crit double default NULL,
  crit_low double default NULL,
  crit_threshold_mode boolean default NULL,
  current_value double default NULL,
  data_source_type smallint NOT NULL default 0,
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
);

--
--  Performance data.
--
CREATE TABLE log_data_bin (
  metric_id int NOT NULL,
  ctime int NOT NULL,
  status smallint NOT NULL default 3,
  value float default NULL,

  FOREIGN KEY (metric_id) REFERENCES rt_metrics (metric_id)
    ON DELETE CASCADE,
  INDEX (metric_id)
);
