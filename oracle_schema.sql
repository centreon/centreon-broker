-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
-- ------------------------------------

-- acknowledgements
-- comments
-- customvariables
-- downtimes
-- eventhandlers
-- flappinghistory
-- hosts
-- hostcommands
-- hostgroups
-- hosts_hostgroups
-- hosts_hosts_dependencies
-- hosts_hosts_parents
-- instances
-- issues
-- issues_issues_parents
-- logs
-- notifications
-- schemaversion
-- services
-- service_dependency
-- service_servicegroup
-- servicecommands
-- servicegroup
-- states


--
-- Holds the current version of the database schema.
--
CREATE TABLE schemaversion (
  software varchar(128) NOT NULL,
  version int NOT NULL
);
INSERT INTO schemaversion (software, version) VALUES ('centreon-broker', 1);


--
-- Store information about Nagios instances.
--
CREATE TABLE instances (
  instance_id int NOT NULL,
  name varchar(255) NOT NULL default 'localhost',

  active_host_checks char(1) default NULL,
  active_service_checks char(1) default NULL,
  address varchar(128) default NULL,
  check_hosts_freshness char(1) default NULL,
  check_services_freshness char(1) default NULL,
  daemon_mode char(1) default NULL,
  description varchar(128) default NULL,
  end_time int default NULL,
  event_handlers char(1) default NULL,
  failure_prediction char(1) default NULL,
  flap_detection char(1) default NULL,
  global_host_event_handler clob default NULL,
  global_service_event_handler clob default NULL,
  last_alive int default NULL,
  last_command_check int default NULL,
  last_log_rotation int default NULL,
  modified_host_attributes int default NULL,
  modified_service_attributes int default NULL,
  notifications char(1) default NULL,
  obsess_over_hosts char(1) default NULL,
  obsess_over_services char(1) default NULL,
  passive_host_checks char(1) default NULL,
  passive_service_checks char(1) default NULL,
  pid int default NULL,
  process_perfdata char(1) default NULL,
  running char(1) default NULL,
  start_time int default NULL,

  PRIMARY KEY (instance_id)
);


--
-- Holds acknowledgedments information.
--
CREATE TABLE acknowledgements (
  acknowledgement_id int NOT NULL,
  entry_time int NOT NULL,
  host_name varchar(255) NOT NULL,
  instance_name varchar(255) NOT NULL,
  service_description varchar(255) default NULL,

  author varchar(64) default NULL,
  comment varchar(255) default NULL,
  notify_contacts char(1) default NULL,
  persistent_comment char(1) default NULL,
  state smallint default NULL,
  sticky char(1) default NULL,
  type smallint default NULL,

  PRIMARY KEY (acknowledgement_id),
  UNIQUE (entry_time, host_name, instance_name, service_description)
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
-- Holds comments information.
--
CREATE TABLE comments (
  comment_id int NOT NULL,
  entry_time int NOT NULL,
  instance_name varchar(255) NOT NULL,
  internal_id int NOT NULL,

  author varchar(64) default NULL,
  data clob default NULL,
  deletion_time int default NULL,
  entry_type smallint default NULL,
  expire_time int default NULL,
  expires char(1) default NULL,
  host_name varchar(255) NOT NULL,
  persistent char(1) default NULL,
  service_description varchar(255) default NULL,
  source smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (id),
  UNIQUE (entry_time, instance_name, internal_id)
);
CREATE SEQUENCE comments_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER comments_trigger
BEFORE INSERT ON comments
FOR EACH ROW
BEGIN
  SELECT comments_seq.nextval INTO :NEW.comment_id FROM dual;
END;
/


--
-- Monitored hosts.
--
CREATE TABLE hosts (
  host_id int NOT NULL,
  host_name varchar(255) NOT NULL,
  instance_id int NOT NULL,

  acknowledged char(1) default NULL,
  acknowledgement_type smallint default NULL,
  action_url varchar(255) default NULL,
  active_checks char(1) default NULL,
  address varchar(75) default NULL,
  alias varchar(100) default NULL,
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
  default_failure_prediction char(1) default NULL,
  default_flap_detection char(1) default NULL,
  default_notify char(1) default NULL,
  default_passive_checks char(1) default NULL,
  default_process_perfdata char(1) default NULL,
  display_name varchar(100) default NULL,
  event_handler varchar(255) default NULL,
  event_handler_enabled char(1) default NULL,
  execution_time double precision default NULL,
  failure_prediction char(1) default NULL,
  first_notification_delay double precision default NULL,
  flap_detection char(1) default NULL,
  flap_detection_on_down char(1) default NULL,
  flap_detection_on_unreachable char(1) default NULL,
  flap_detection_on_up char(1) default NULL,
  flapping char(1) default NULL,
  freshness_threshold double precision default NULL,
  high_flap_threshold double precision default NULL,
  icon_image varchar(255) default NULL,
  icon_image_alt varchar(255) default NULL,
  initial_state varchar(18) default NULL,
  last_check int default NULL,
  last_hard_state smallint default NULL,
  last_hard_state_change int default NULL,
  last_notification int default NULL,
  last_state_change int default NULL,
  last_time_down int default NULL,
  last_time_unreachable int default NULL,
  last_time_up int default NULL,
  last_update int default NULL,
  latency double precision default NULL,
  low_flap_threshold double precision default NULL,
  max_check_attempts smallint default NULL,
  modified_attributes int default NULL,
  next_check int default NULL,
  next_host_notification int default NULL,
  no_more_notifications char(1) default NULL,
  notes varchar(255) default NULL,
  notes_url varchar(255) default NULL,
  notification_interval double precision default NULL,
  notification_number smallint default NULL,
  notification_period varchar(75) default NULL,
  notify char(1) default NULL,
  notify_on_down char(1) default NULL,
  notify_on_downtime char(1) default NULL,
  notify_on_flapping char(1) default NULL,
  notify_on_recovery char(1) default NULL,
  notify_on_unreachable char(1) default NULL,
  obsess_over_host char(1) default NULL,
  output clob default NULL,
  passive_checks char(1) default NULL,
  percent_state_change double precision default NULL,
  perfdata clob default NULL,
  process_perfdata char(1) default NULL,
  retain_nonstatus_information char(1) default NULL,
  retain_status_information char(1) default NULL,
  retry_interval double precision default NULL,
  scheduled_downtime_depth smallint default NULL,
  should_be_scheduled char(1) default NULL,
  stalk_on_down char(1) default NULL,
  stalk_on_unreachable char(1) default NULL,
  stalk_on_up char(1) default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  statusmap_image varchar(255) default NULL,

  UNIQUE (host_id),
  UNIQUE (host_name, instance_id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
);


--
-- Host groups.
--
CREATE TABLE hostgroups (
  hostgroup_id int NOT NULL,
  name varchar(255) NOT NULL,
  instance_id int NOT NULL,

  action_url varchar(160) default NULL,
  alias varchar(255) default NULL,
  notes varchar(160) default NULL,
  notes_url varchar(160) default NULL,

  PRIMARY KEY (hostgroup_id),
  UNIQUE (name, instance_id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE hostgroups_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER hostgroups_trigger
BEFORE INSERT ON hosts
FOR EACH ROW
BEGIN
  SELECT hostgroups_seq.nextval INTO :NEW.hostgroup_id FROM dual;
END;
/


--
-- Relationships between hosts and host groups.
--
CREATE TABLE hosts_hostgroups (
  host_id int NOT NULL,
  hostgroup_id int NOT NULL,

  UNIQUE (host_id, hostgroup_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (hostgroup_id) REFERENCES hostgroups (id)
    ON DELETE CASCADE
);


--
-- Hosts dependencies.
--
CREATE TABLE host_dependency (
  dependent_host_id int NOT NULL,
  host_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent char(1) default NULL,
  notification_failure_options varchar(15) default NULL,

  UNIQUE (dependent_host_id, host_id),
  FOREIGN KEY (dependent_host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);


--
-- Hosts parenting relationships.
--
CREATE TABLE hosts_hosts_parents (
  child_id int NOT NULL,
  parent_id int NOT NULL,

  UNIQUE (child_id, parent_id),
  FOREIGN KEY (child_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (parent_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);


--
-- Correlated issues.
--
CREATE TABLE issues (
  issue_id int NOT NULL,
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
-- Nagios logs.
--
CREATE TABLE logs (
  id int NOT NULL,
  ctime int default NULL,
  host_name varchar(255) default NULL,
  instance_name varchar(255) NOT NULL,
  issue_id int default NULL,
  msg_type char(1) default NULL,
  notification_cmd varchar(255) default NULL,
  notification_contact varchar(255) default NULL,
  output clob default NULL,
  retry int default NULL,
  service_description varchar(255) default NULL,
  status char(1) default NULL,
  type smallint default NULL,
  PRIMARY KEY (id),
  CONSTRAINT status_cons CHECK (status IN ('0', '1', '2', '3', '4'))
);
CREATE SEQUENCE logs_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER logs_trigger
BEFORE INSERT ON logs
FOR EACH ROW
BEGIN
  SELECT logs_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Downtimes.
--
CREATE TABLE downtimes (
  id int NOT NULL,
  entry_time int default NULL,
  instance_name varchar(255) NOT NULL,
  internal_id int default NULL,
  author_name varchar(64) default NULL,
  comment_data clob default NULL,
  downtime_type smallint default NULL,
  duration int default NULL,
  end_time int default NULL,
  fixed char(1) default NULL,
  host_name varchar(255) NOT NULL,
  service_description varchar(255) default NULL,
  start_time int default NULL,
  triggered_by int default NULL,
  was_cancelled char(1) default NULL,
  was_started char(1) default NULL,
  PRIMARY KEY (id),
  UNIQUE (entry_time, instance_name, internal_id)
);
CREATE SEQUENCE downtimes_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER downtimes_trigger
BEFORE INSERT ON downtimes
FOR EACH ROW
BEGIN
  SELECT downtimes_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Monitored services.
--
CREATE TABLE services (
  host_id int NOT NULL,
  service_description varchar(255) NOT NULL,
  service_id int default NULL,

  acknowledgement_type smallint default NULL,
  action_url varchar(255) default NULL,
  active_checks_enabled char(1) default NULL,
  check_command clob default NULL,
  check_freshness char(1) default NULL,
  check_interval double precision default NULL,
  check_period varchar(75) default NULL,
  check_type smallint default NULL,
  command_line clob default NULL,
  current_attempt smallint default NULL,
  current_notification_number smallint default NULL,
  current_state smallint default NULL,
  default_active_checks_enabled char(1) default NULL,
  default_event_handler_enabled char(1) default NULL,
  default_failure_prediction char(1) default NULL,
  default_flap_detection_enabled char(1) default NULL,
  default_notifications_enabled char(1) default NULL,
  default_passive_checks_enabled char(1) default NULL,
  default_process_perfdata char(1) default NULL,
  display_name varchar(160) default NULL,
  event_handler varchar(255) default NULL,
  event_handler_enabled char(1) default NULL,
  execution_time double precision default NULL,
  failure_prediction_enabled char(1) default NULL,
  failure_prediction_options varchar(64) default NULL,
  first_notification_delay double precision default NULL,
  flap_detection_enabled char(1) default NULL,
  flap_detection_on_critical char(1) default NULL,
  flap_detection_on_ok char(1) default NULL,
  flap_detection_on_unknown char(1) default NULL,
  flap_detection_on_warning char(1) default NULL,
  freshness_threshold double precision default NULL,
  has_been_checked char(1) default NULL,
  high_flap_threshold double precision default NULL,
  icon_image varchar(255) default NULL,
  icon_image_alt varchar(255) default NULL,
  initial_state varchar(1) default NULL,
  is_flapping char(1) default NULL,
  is_volatile char(1) default NULL,
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
  modified_attributes int default NULL,
  next_check int default NULL,
  next_notification int default NULL,
  no_more_notifications char(1) default NULL,
  notes varchar(255) default NULL,
  notes_url varchar(255) default NULL,
  notification_interval double precision default NULL,
  notification_period varchar(75) default NULL,
  notifications_enabled char(1) default NULL,
  notify_on_critical char(1) default NULL,
  notify_on_downtime char(1) default NULL,
  notify_on_flapping char(1) default NULL,
  notify_on_recovery char(1) default NULL,
  notify_on_unknown char(1) default NULL,
  notify_on_warning char(1) default NULL,
  obsess_over_service char(1) default NULL,
  output clob default NULL,
  passive_checks_enabled char(1) default NULL,
  percent_state_change double precision default NULL,
  perf_data clob default NULL,
  problem_has_been_acknowledged char(1) default NULL,
  process_perfdata char(1) default NULL,
  retain_nonstatus_information char(1) default NULL,
  retain_status_information char(1) default NULL,
  retry_interval double precision default NULL,
  scheduled_downtime_depth smallint default NULL,
  should_be_scheduled char(1) default NULL,
  stalk_on_critical char(1) default NULL,
  stalk_on_ok char(1) default NULL,
  stalk_on_unknown char(1) default NULL,
  stalk_on_warning char(1) default NULL,
  state_type smallint default NULL,

  UNIQUE (host_id, service_description),
  UNIQUE (service_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE services_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER services_trigger
BEFORE INSERT ON services
FOR EACH ROW
BEGIN
  SELECT services_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Groups of services.
--
CREATE TABLE servicegroups (
  id int NOT NULL,
  instance_id int NOT NULL,
  servicegroup_name varchar(255) NOT NULL,
  action_url varchar(160) default NULL,
  alias varchar(255) default NULL,
  notes varchar(160) default NULL,
  notes_url varchar(160) default NULL,
  PRIMARY KEY (id),
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE servicegroups_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER servicegroups_trigger
BEFORE INSERT ON servicegroups
FOR EACH ROW
BEGIN
  SELECT servicegroups_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Relationships between services and service groups.
--
CREATE TABLE service_servicegroup (
  service_id int NOT NULL,
  servicegroup_id int NOT NULL,
  UNIQUE (service_id, servicegroup_id),
  FOREIGN KEY (service_id) REFERENCES services (service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (servicegroup_id) REFERENCES servicegroups (id)
    ON DELETE CASCADE
);


--
-- Services dependencies.
--
CREATE TABLE service_dependency (
  id int NOT NULL,
  dependent_service_id int NOT NULL,
  service_id int NOT NULL,
  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent char(1) default NULL,
  notification_failure_options varchar(15) default NULL,
  PRIMARY KEY (id),
  FOREIGN KEY (dependent_service_id) REFERENCES services (service_id)
    ON DELETE CASCADE,
  FOREIGN KEY (service_id) REFERENCES services (service_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE service_dependency_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER service_dependency_trigger
BEFORE INSERT ON service_dependency
FOR EACH ROW
BEGIN
  SELECT service_dependency_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Custom variables.
--
CREATE TABLE customvariables (
  id int NOT NULL,
  config_type smallint default NULL,
  has_been_modified char(1) default NULL,
  host_id int default NULL,
  service_id int default NULL,
  status_update_time int NOT NULL,
  varname varchar(255) default NULL,
  varvalue varchar(255) default NULL,
  PRIMARY KEY (id)
);
CREATE SEQUENCE customvariables_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER customvariables_trigger
BEFORE INSERT ON customvariables
FOR EACH ROW
BEGIN
  SELECT customvariables_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Event handlers.
--
CREATE TABLE eventhandlers (
  id int NOT NULL,
  command_args varchar(255) default NULL,
  command_line varchar(255) default NULL,
  early_timeout smallint default NULL,
  end_time int default NULL,
  eventhandler_type smallint default NULL,
  execution_time double precision default NULL,
  host_id int default NULL,
  output varchar(255) default NULL,
  return_code smallint default NULL,
  service_id int default NULL,
  start_time int default NULL,
  state smallint default NULL,
  state_type smallint default NULL,
  timeout smallint default NULL,
  PRIMARY KEY (id)
);
CREATE SEQUENCE eventhandlers_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER eventhandlers_trigger
BEFORE INSERT ON eventhandlers
FOR EACH ROW
BEGIN
  SELECT eventhandlers_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
--  Notifications.
--
CREATE TABLE notifications (
  id int NOT NULL,
  ack_author varchar(255) default NULL,
  ack_data clob default NULL,
  command_name varchar(255) default NULL,
  contact_name varchar(255) default NULL,
  contacts_notified char(1) default NULL,
  end_time int default NULL,
  escalated char(1) default NULL,
  host_id int default NULL,
  notification_type int default NULL,
  output clob default NULL,
  reason_type int default NULL,
  service_id int default NULL,
  start_time int default NULL,
  state int default NULL,
  PRIMARY KEY (id)
);
CREATE SEQUENCE notifications_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER notifications_trigger
BEFORE INSERT ON notifications
FOR EACH ROW
BEGIN
  SELECT notifications_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Historization of flapping status.
--
CREATE TABLE flappinghistory (
  id int NOT NULL,
  comment_time int default NULL,
  event_time int default NULL,
  event_type smallint default NULL,
  flapping_type smallint default NULL,
  high_threshold double precision default NULL,
  host_id int default NULL,
  instance_id int default NULL,
  internal_comment_id int default NULL,
  low_threshold double precision default NULL,
  percent_state_change double precision default NULL,
  reason_type smallint default NULL,
  service_id int default NULL,
  PRIMARY KEY (id)
);
CREATE SEQUENCE flappinghistory_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER flappinghistory_trigger
BEFORE INSERT ON flappinghistory
FOR EACH ROW
BEGIN
  SELECT flappinghistory_seq.nextval INTO :NEW.id FROM dual;
END;
/


--
-- Executed commands related to hosts.
--
CREATE TABLE hostcommands (
  host_id int default NULL,
  check_command clob default NULL,
  event_handler_command clob default NULL,
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);


--
-- Executed commands related to services.
--
CREATE TABLE services_commands (
  service_id int default NULL,
  check_command clob,
  event_handler_command clob,
  FOREIGN KEY (service_id) REFERENCES services (service_id)
    ON DELETE CASCADE
);


--
-- States of checkpoints.
--
CREATE TABLE states (
  state_id int NOT NULL,

  end_time int default NULL,
  host_id int NOT NULL,
  service_id int default NULL,
  start_time int NOT NULL,
  state int NOT NULL,
  
  PRIMARY KEY (state_id)
);
CREATE SEQUENCE states_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER states_trigger
BEFORE INSERT ON states
FOR EACH ROW
BEGIN
  SELECT states_seq.nextval INTO :NEW.state_id FROM dual;
END;
/
