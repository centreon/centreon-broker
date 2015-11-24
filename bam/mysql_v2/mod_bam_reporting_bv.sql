--
-- Business Views.
--
CREATE TABLE mod_bam_reporting_bv (
  bv_id int NOT NULL auto_increment,
  bv_name varchar(45) default NULL,

  bv_description text default NULL,

  PRIMARY KEY (bv_id),
  UNIQUE (bv_name)
) ENGINE=InnoDB CHARACTER SET utf8;
