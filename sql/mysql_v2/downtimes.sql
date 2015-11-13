--
-- Downtimes.
--
CREATE TABLE downtimes (
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

  PRIMARY KEY (downtime_id),
  UNIQUE (entry_time, host_id, service_id),
  INDEX (host_id),
  INDEX (instance_id),
  INDEX (entry_time),
  INDEX (host_id, start_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (instance_id) REFERENCES instances (instance_id)
    ON DELETE SET NULL
) ENGINE=InnoDB;
