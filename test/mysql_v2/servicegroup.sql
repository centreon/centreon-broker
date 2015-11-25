--
-- Service group configuration table.
--
CREATE TABLE servicegroup (
  sg_id int NOT NULL auto_increment,

  sg_name varchar(200) default NULL,
  sg_alias varchar(200) default NULL,
  sg_comment text,
  sg_activate enum('0','1') NOT NULL default '1',

  PRIMARY KEY (sg_id),
  INDEX (sg_name),
  INDEX (sg_alias)
) ENGINE=InnoDB CHARACTER SET utf8;
