--
-- Business Activities types.
--
CREATE TABLE cfg_bam_ba_types (
  ba_type_id int NOT NULL auto_increment,
  name varchar(255) default NULL,
  slug varchar(255) default NULL,
  description varchar(255) default NULL,

  PRIMARY KEY (ba_type_id),
  KEY (name)
) ENGINE=InnoDB CHARACTER SET utf8;
