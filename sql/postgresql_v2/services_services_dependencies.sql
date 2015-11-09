--
-- Services dependencies.
--
CREATE TABLE services_services_dependencies (
  dependent_host_id int NOT NULL,
  dependent_service_id int NOT NULL,
  host_id int NOT NULL,
  service_id int NOT NULL,

  dependency_period varchar(75) default NULL,
  execution_failure_options varchar(15) default NULL,
  inherits_parent boolean default NULL,
  notification_failure_options varchar(15) default NULL,

  UNIQUE (dependent_host_id, dependent_service_id, host_id, service_id),
  INDEX (host_id),
--  FOREIGN KEY (dependent_host_id, dependent_service_id) REFERENCES services (host_id, service_id)
--    ON DELETE CASCADE,
  FOREIGN KEY (dependent_host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE,
--  FOREIGN KEY (host_id, service_id) REFERENCES services (host_id, service_id)
--    ON DELETE CASCADE
  FOREIGN KEY (host_id) REFERENCES hosts (host_id)
    ON DELETE CASCADE
);
