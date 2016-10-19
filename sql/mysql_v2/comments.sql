--
-- Holds comments information.
--
CREATE TABLE comments (
  comment_id int NOT NULL auto_increment,
  entry_time int NOT NULL,
  host_id int NOT NULL,
  service_id int default NULL,

  author varchar(64) default NULL,
  data text default NULL,
  deletion_time int default NULL,
  entry_type smallint default NULL,
  expire_time int default NULL,
  expires boolean default NULL,
  instance_id int default NULL,
  internal_id int NOT NULL,
  persistent boolean default NULL,
  source smallint default NULL,
  type smallint default NULL,

  PRIMARY KEY (comment_id),
  INDEX (internal_id),
  INDEX (host_id),
  INDEX (instance_id),
  UNIQUE (entry_time, host_id, service_id, instance_id, internal_id),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE SET NULL
) ENGINE=InnoDB;
