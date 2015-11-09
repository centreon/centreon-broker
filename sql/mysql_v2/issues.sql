--
-- Correlated issues.
--
CREATE TABLE issues (
  issue_id int NOT NULL auto_increment,
  host_id int default NULL,
  service_id int default NULL,
  start_time int NOT NULL,

  ack_time int default NULL,
  end_time int default NULL,

  PRIMARY KEY (issue_id),
  UNIQUE (host_id, service_id, start_time),
  INDEX (start_time),
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;
