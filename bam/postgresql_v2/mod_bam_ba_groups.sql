--
-- BA Groups (aka BV).
--
CREATE TABLE mod_bam_ba_groups (
  id_ba_group serial,

  ba_group_name varchar(255) default NULL,
  ba_group_description varchar(255) default NULL,
  visible enum('0', '1') NOT NULL default '1',

  PRIMARY KEY (id_ba_group)
);
