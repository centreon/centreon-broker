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
