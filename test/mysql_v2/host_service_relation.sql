--
-- Associate services with hosts.
--
CREATE TABLE host_service_relation (
  hsr_id int NOT NULL auto_increment,

  hostgroup_hg_id int default NULL,
  host_host_id int default NULL,
  servicegroup_sg_id int default NULL,
  service_service_id int default NULL,

  PRIMARY KEY (hsr_id),
  INDEX (hostgroup_hg_id),
  INDEX (host_host_id),
  INDEX (servicegroup_sg_id),
  INDEX (service_service_id),
  INDEX (host_host_id,service_service_id),
  INDEX (host_host_id,service_service_id),
  FOREIGN KEY (hostgroup_hg_id) REFERENCES hostgroup (hg_id)
    ON DELETE CASCADE,
  FOREIGN KEY (host_host_id) REFERENCES host (host_id)
    ON DELETE CASCADE,
  FOREIGN KEY (servicegroup_sg_id) REFERENCES servicegroup (sg_id)
    ON DELETE CASCADE,
  FOREIGN KEY (service_service_id) REFERENCES service (service_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
