-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--         Performance data          --
-- ------------------------------------

-- data_bin
-- flappingstatuses
-- hoststateevents
-- index_data
-- issues
-- issues_issues_parents
-- metrics
-- notifications
-- schemaversion
-- servicestateevents


--
-- Holds the current version of the database schema.
--
CREATE TABLE schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
);
INSERT INTO schemaversion (software, version) VALUES ('centreon-broker', 2);


--
-- Historization of flapping statuses.
--
CREATE TABLE flappingstatuses (
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
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;


--
-- Correlated issues.
--
CREATE TABLE issues (
  issue_id serial,
  host_id int default NULL,
  service_id int default NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,

  PRIMARY KEY (issue_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);


--
-- Issues parenting.
--
CREATE TABLE issues_issues_parents (
  child_id int NOT NULL,
  end_time int default NULL,
  start_time int NOT NULL,
  parent_id int NOT NULL,

  FOREIGN KEY (child_id) REFERENCES issues (issue_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_id) REFERENCES issues (issue_id)
    ON DELETE CASCADE
);


--
--  Notifications.
--
CREATE TABLE notifications (
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
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);


--
--  Host states.
--
CREATE TABLE hoststateevents (
  host_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime boolean default NULL,
  state int default NULL,

  UNIQUE (host_id, start_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);


--
--  Service states.
--
CREATE TABLE servicestateevents (
  host_id int NOT NULL,
  service_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime boolean default NULL,
  state int default NULL,

  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id, service_id) REFERENCES services (host_id, service_id)
    ON DELETE CASCADE
);

--
--  Base performance data index.
--
CREATE TABLE index_data (
  id serial,
  host_id int NOT NULL,
  service_id int default NULL,

  check_interval int default NULL,
  hidden enum('0', '1') NOT NULL default '0',
  host_name varchar(255) default NULL,
  locked enum('0', '1') NOT NULL default '0',
  must_be_rebuild enum('0', '1', '2') NOT NULL default '0',
  rrd_retention int default NULL,
  service_description varchar(255) default NULL,
  special enum('0', '1') NOT NULL default '0',
  storage_type enum('0', '1', '2') NOT NULL default '2',
  to_delete int NOT NULL default 0,
  trashed enum('0', '1') NOT NULL default '0',

  PRIMARY KEY (id),
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
CREATE TABLE metrics (
  metric_id serial,
  index_id int NOT NULL,
  metric_name varchar(255) NOT NULL,

  crit double default NULL,
  crit_low double default NULL,
  crit_threshold_mode boolean default NULL,
  current_value double default NULL,
  data_source_type enum('0', '1', '2', '3') NOT NULL default '0',
  hidden enum('0', '1') NOT NULL default '0',
  locked enum('0', '1') NOT NULL default '0',
  min double default NULL,
  max double default NULL,
  to_delete int NOT NULL default 0,
  unit_name varchar(32) default NULL,
  warn double default NULL,
  warn_low double default NULL,
  warn_threshold_mode boolean default NULL,

  PRIMARY KEY (metric_id),
  UNIQUE KEY (index_id, metric_name),
  FOREIGN KEY (index_id) REFERENCES index_data (id)
    ON DELETE CASCADE,
  INDEX (index_id)
);

--
--  Performance data.
--
CREATE TABLE data_bin (
  id_metric int NOT NULL,
  ctime int NOT NULL,
  status enum('0', '1', '2', '3', '4') NOT NULL default '3',
  value float default NULL,

  FOREIGN KEY (id_metric) REFERENCES metrics (metric_id)
    ON DELETE CASCADE,
  INDEX (id_metric)
);
