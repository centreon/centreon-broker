--
-- Event handlers.
--
CREATE TABLE rt_eventhandlers (
  eventhandler_id int NOT NULL auto_increment,
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

  PRIMARY KEY (eventhandler_id),
  UNIQUE (host_id, service_id, start_time),
  FOREIGN KEY (host_id) REFERENCES rt_hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;
