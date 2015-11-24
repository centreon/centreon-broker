--
-- Meta Services Relationships.
--
CREATE TABLE cfg_meta_services_relations (
  meta_id int NOT NULL,
  host_id int NOT NULL,
  metric_id int NOT NULL,
  msr_comment text,
  activate enum('0','1') default NULL,

  UNIQUE (meta_id, host_id, metric_id),
  FOREIGN KEY (meta_id) REFERENCES cfg_meta_services (meta_id) ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
