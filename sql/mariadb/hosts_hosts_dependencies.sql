--
-- Hosts dependencies.
--
CREATE TABLE hosts_hosts_dependencies (
  dependent_host_id int NOT NULL,
  host_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent boolean default NULL,
  notification_failure_options varchar(15) default NULL,

  UNIQUE (dependent_host_id, host_id),
  INDEX (host_id),
  FOREIGN KEY (dependent_host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
) ENGINE=InnoDB;
