-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--          Real-time data           --
--              Logs                 --
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
-- log_logs
-- rt_metrics
-- rt_modules
-- rt_notifications
-- rt_schemaversion
-- rt_services
-- rt_services_services_dependencies
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
-- Hosts dependencies.
--
CREATE TABLE rt_hosts_hosts_dependencies (
  dependent_host_id int NOT NULL,
  host_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent char(1) default NULL,
  notification_failure_options varchar(15) default NULL,

  FOREIGN KEY (dependent_host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);


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
);


--
-- Monitored services.
--
CREATE TABLE rt_services (
  host_id int NOT NULL,
  description varchar(255) NOT NULL,
  service_id int NOT NULL,

  acknowledged char(1) default NULL,
  acknowledgement_type smallint default NULL,
  active_checks char(1) default NULL,
  check_attempt smallint default NULL,
  check_command clob default NULL,
  check_freshness char(1) default NULL,
  check_interval double precision default NULL,
  check_period varchar(75) default NULL,
  check_type smallint default NULL,
  checked char(1) default NULL,
  command_line clob default NULL,
  default_active_checks char(1) default NULL,
  default_event_handler_enabled char(1) default NULL,
  default_flap_detection char(1) default NULL,
  default_notifications char(1) default NULL,
  enabled char(1) default 1 NOT NULL,
  event_handler varchar(255) default NULL,
  event_handler_enabled char(1) default NULL,
  execution_time double precision default NULL,
  first_notification_delay double precision default NULL,
  flap_detection char(1) default NULL,
  flap_detection_on_critical char(1) default NULL,
  flap_detection_on_ok char(1) default NULL,
  flap_detection_on_unknown char(1) default NULL,
  flap_detection_on_warning char(1) default NULL,
  flapping char(1) default NULL,
  freshness_threshold double precision default NULL,
  high_flap_threshold double precision default NULL,
  last_check int default NULL,
  last_hard_state smallint default NULL,
  last_hard_state_change int default NULL,
  last_notification int default NULL,
  last_state_change int default NULL,
  last_time_critical int default NULL,
  last_time_ok int default NULL,
  last_time_unknown int default NULL,
  last_time_warning int default NULL,
  last_update int default NULL,
  latency double precision default NULL,
  low_flap_threshold double precision default NULL,
  max_check_attempts smallint default NULL,
  next_check int default NULL,
  next_notification int default NULL,
  no_more_notifications char(1) default NULL,
  notification_interval double precision default NULL,
  notification_number smallint default NULL,
  notification_period varchar(75) default NULL,
  notify char(1) default NULL,
  notify_on_critical char(1) default NULL,
  notify_on_downtime char(1) default NULL,
  notify_on_flapping char(1) default NULL,
  notify_on_recovery char(1) default NULL,
  notify_on_unknown char(1) default NULL,
  notify_on_warning char(1) default NULL,
  obsess_over_service char(1) default NULL,
  output clob default NULL,
  percent_state_change double precision default NULL,
  perfdata clob default NULL,
  real_state smallint default NULL,
  retry_interval double precision default NULL,
  scheduled_downtime_depth smallint NOT NULL default 0,
  should_be_scheduled char(1) default NULL,
  stalk_on_critical char(1) default NULL,
  stalk_on_ok char(1) default NULL,
  stalk_on_unknown char(1) default NULL,
  stalk_on_warning char(1) default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  volatile char(1) default NULL,

  UNIQUE (host_id, service_id),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  INDEX (acknowledged),
  INDEX (enabled),
  INDEX (last_hard_state),
  INDEX (last_hard_state_change),
  INDEX (last_state_change),
  INDEX (scheduled_downtime_depth),
  INDEX (state),
  INDEX (state_type)
);


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
  inherits_parent char(1) default NULL,
  notification_failure_options varchar(15) default NULL,

  FOREIGN KEY (dependent_host_id, dependent_service_id) REFERENCES rt_services (host_id, service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id, service_id) REFERENCES rt_services (host_id, service_id)
    ON DELETE CASCADE
);


--
-- Holds acknowledgedments information.
--
CREATE TABLE rt_acknowledgements (
  acknowledgement_id int NOT NULL,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  author varchar(64) default NULL,
  comment_data varchar(255) default NULL,
  deletion_time int default NULL,
  instance_id int default NULL,
  notify_contacts char(1) default NULL,
  persistent_comment char(1) default NULL,
  state smallint default NULL,
  sticky char(1) default NULL,
  type smallint default NULL,

  PRIMARY KEY (acknowledgement_id),
  UNIQUE (entry_time, host_id, service_id),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES rt_instances (instance_id)
    ON DELETE SET NULL
);
CREATE SEQUENCE acknowledgements_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER acknowledgements_trigger
BEFORE INSERT ON acknowledgements
FOR EACH ROW
BEGIN
  SELECT acknowledgements_seq.nextval INTO :NEW.acknowledgement_id FROM dual;
END;
/


--
-- Custom variables.
--
CREATE TABLE rt_customvariables (
  customvariable_id int NOT NULL,
  host_id int default NULL,
  name varchar(255) default NULL,
  service_id int default NULL,

  default_value varchar(255) default NULL,
  modified char(1) default NULL,
  type smallint default NULL,
  update_time int default NULL,
  value varchar(255) default NULL,

  PRIMARY KEY (customvariable_id),
  UNIQUE (host_id, name, service_id)
);
CREATE SEQUENCE customvariables_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER customvariables_trigger
BEFORE INSERT ON customvariables
FOR EACH ROW
BEGIN
  SELECT customvariables_seq.nextval INTO :NEW.customvariable_id FROM dual;
END;
/


--
-- Downtimes.
--
CREATE TABLE rt_downtimes (
  downtime_id int NOT NULL,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  actual_end_time int default NULL,
  actual_start_time int default NULL,
  author varchar(64) default NULL,
  cancelled char(1) default NULL,
  comment_data clob default NULL,
  deletion_time int default NULL,
  duration int default NULL,
  end_time int default NULL,
  fixed char(1) default NULL,
  instance_id int default NULL,
  internal_id int default NULL,
  start_time int default NULL,
  started char(1) default NULL,
  triggered_by int default NULL,
  type smallint default NULL,
  is_recurring char(1) default NULL,
  recurring_interval int default NULL,
  recurring_timeperiod varchar(200) default NULL,

  PRIMARY KEY (downtime_id),
  UNIQUE (entry_time, host_id, service_id),
  UNIQUE (entry_time, host_id, internal_id),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES rt_instances (instance_id)
    ON DELETE SET NULL
);
CREATE SEQUENCE downtimes_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER downtimes_trigger
BEFORE INSERT ON downtimes
FOR EACH ROW
BEGIN
  SELECT downtimes_seq.nextval INTO :NEW.downtime_id FROM dual;
END;
/


--
-- Event handlers.
--
CREATE TABLE rt_eventhandlers (
  host_id int default NULL,
  service_id int default NULL,
  start_time int default NULL,

  command_args varchar(255) default NULL,
  command_line varchar(255) default NULL,
  early_timeout char(1) default NULL,
  end_time int default NULL,
  eventhandler_type smallint default NULL,
  execution_time double precision default NULL,
  output varchar(255) default NULL,
  return_code smallint default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  timeout smallint default NULL,

  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);


--
-- Historization of flapping statuses.
--
CREATE TABLE rt_flappingstatuses (
  flappingstatus_id int NOT NULL,
  host_id int default NULL,
  service_id int default NULL,
  event_time int default NULL,

  comment_time int default NULL,
  event_type smallint default NULL,
  high_threshold double precision default NULL,
  instance_id int default NULL,
  internal_comment_id int default NULL,
  low_threshold double precision default NULL,
  percent_state_change double precision default NULL,
  reason_type smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (flappingstatus_id),
  UNIQUE (host_id, service_id, event_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE flappingstatuses_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER flappingstatuses_trigger
BEFORE INSERT ON flappingstatuses
FOR EACH ROW
BEGIN
  SELECT flappingstatuses_seq.nextval INTO :NEW.flappingstatus_id FROM dual;
END;
/


--
-- Correlated issues.
--
CREATE TABLE rt_issues (
  issue_id int NOT NULL,
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
CREATE SEQUENCE issues_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER issues_trigger
BEFORE INSERT ON issues
FOR EACH ROW
BEGIN
  SELECT issues_seq.nextval INTO :NEW.issue_id FROM dual;
END;
/


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
-- Nagios logs.
--
CREATE TABLE log_logs (
  log_id int NOT NULL,

  ctime int default NULL,
  host_id default NULL,
  host_name varchar(255) default NULL,
  instance_name varchar(255) NOT NULL,
  issue_id int default NULL,
  msg_type char(1) default NULL,
  notification_cmd varchar(255) default NULL,
  notification_contact varchar(255) default NULL,
  output clob default NULL,
  retry int default NULL,
  service_description varchar(255) default NULL,
  service_id int default NULL,
  status char(1) default NULL,
  type smallint default NULL,

  PRIMARY KEY (log_id),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE SET NULL,
  CONSTRAINT status_cons CHECK (status IN ('0', '1', '2', '3', '4'))
);
CREATE SEQUENCE logs_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER logs_trigger
BEFORE INSERT ON logs
FOR EACH ROW
BEGIN
  SELECT logs_seq.nextval INTO :NEW.log_id FROM dual;
END;
/


--
-- Nagios modules.
--
CREATE TABLE rt_modules (
  module_id int NOT NULL,
  instance_id int NOT NULL,

  args varchar(255) default NULL,
  filename varchar(255) default NULL,
  loaded char(1) default NULL,
  should_be_loaded char(1) default NULL,

  PRIMARY KEY (module_id),
  FOREIGN KEY (instance_id) REFERENCES rt_instances (instance_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE modules_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER modules_trigger
BEFORE INSERT ON modules
FOR EACH ROW
BEGIN
  SELECT modules_seq.nextval INTO :NEW.module_id FROM dual;
END;
/


--
--  Notifications.
--
CREATE TABLE rt_notifications (
  notification_id int NOT NULL,
  host_id int default NULL,
  start_time int default NULL,
  service_id int default NULL,

  ack_author varchar(255) default NULL,
  ack_data clob default NULL,
  command_name varchar(255) default NULL,
  contact_name varchar(255) default NULL,
  contacts_notified char(1) default NULL,
  end_time int default NULL,
  escalated char(1) default NULL,
  output clob default NULL,
  reason_type int default NULL,
  state int default NULL,
  type int default NULL,

  PRIMARY KEY (notification_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE notifications_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER notifications_trigger
BEFORE INSERT ON notifications
FOR EACH ROW
BEGIN
  SELECT notifications_seq.nextval INTO :NEW.notification_id FROM dual;
END;
/


--
--  Host states.
--
CREATE TABLE rt_hoststateevents (
  host_id int NOT NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,
  in_downtime char(1) default NULL,
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
  in_downtime char(1) default NULL,
  state int default NULL,

  UNIQUE (host_id, service_id, start_time)
);


--
--  Base performance data index.
--
CREATE TABLE rt_index_data (
  index_id int NOT NULL,
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
CREATE SEQUENCE index_data_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER index_data_trigger
BEFORE INSERT ON index_data
FOR EACH ROW
BEGIN
  SELECT index_data_seq.nextval INTO :NEW.index_id FROM dual;
END;
/

--
--  Metrics.
--
CREATE TABLE rt_metrics (
  metric_id int NOT NULL,
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
CREATE SEQUENCE metrics_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER metrics_trigger
BEFORE INSERT ON metrics
FOR EACH ROW
BEGIN
  SELECT metrics_seq.nextval INTO :NEW.metric_id FROM dual;
END;
/

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
