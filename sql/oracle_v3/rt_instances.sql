--
-- Real-time information about Centreon Engine instances.
--
CREATE TABLE rt_instances (
  instance_id int NOT NULL,
  name varchar(255) default 'localhost' NOT NULL,

  address varchar(128) default NULL,
  check_hosts_freshness char(1) default NULL,
  check_services_freshness char(1) default NULL,
  deleted char(1) NOT NULL default 0,
  description varchar(128) default NULL,
  end_time int default NULL,
  engine varchar(64) default NULL,
  event_handlers char(1) default NULL,
  flap_detection char(1) default NULL,
  global_host_event_handler clob default NULL,
  global_service_event_handler clob default NULL,
  last_alive int default NULL,
  last_command_check int default NULL,
  notifications char(1) default NULL,
  obsess_over_hosts char(1) default NULL,
  obsess_over_services char(1) default NULL,
  outdated boolean NOT NULL default 0,
  pid int default NULL,
  running char(1) default NULL,
  start_time int default NULL,
  version varchar(16) default NULL,

  PRIMARY KEY (instance_id)
);
