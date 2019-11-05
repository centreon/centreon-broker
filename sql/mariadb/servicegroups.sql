--
-- Groups of services.
--
CREATE TABLE servicegroups (
  servicegroup_id int NOT NULL,
  name varchar(255) NOT NULL,

  PRIMARY KEY (servicegroup_id)
) ENGINE=InnoDB;
