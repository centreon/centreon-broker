--
-- Real-time information about Centreon Engine instances.
--
CREATE TABLE instances (
  instance_id int NOT NULL,
  name varchar(255) NOT NULL default 'localhost',

  active_host_checks boolean default NULL,
  active_service_checks boolean default NULL,
  address varchar(128) default NULL,
  check_hosts_freshness boolean default NULL,
  check_services_freshness boolean default NULL,
  daemon_mode boolean default NULL,
  deleted boolean NOT NULL default false,
  description varchar(128) default NULL,
  end_time int default NULL,
  engine varchar(64) default NULL,
  event_handlers boolean default NULL,
  failure_prediction boolean default NULL,
  flap_detection boolean default NULL,
  global_host_event_handler text default NULL,
  global_service_event_handler text default NULL,
  last_alive int default NULL,
  last_command_check int default NULL,
  last_log_rotation int default NULL,
  modified_host_attributes int default NULL,
  modified_service_attributes int default NULL,
  notifications boolean default NULL,
  obsess_over_hosts boolean default NULL,
  obsess_over_services boolean default NULL,
  outdated boolean NOT NULL default false,
  passive_host_checks boolean default NULL,
  passive_service_checks boolean default NULL,
  pid int default NULL,
  process_perfdata boolean default NULL,
  running boolean default NULL,
  start_time int default NULL,
  version varchar(16) default NULL,

  PRIMARY KEY (instance_id)
) ENGINE=InnoDB;
