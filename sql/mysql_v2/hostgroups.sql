--
-- Groups of hosts.
--
CREATE TABLE hostgroups (
  hostgroup_id int NOT NULL,
  name varchar(255) NOT NULL,

  PRIMARY KEY (hostgroup_id)
) ENGINE=InnoDB;
