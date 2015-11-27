--
-- Host group configuration table.
--
CREATE TABLE hostgroup (
  hg_id int NOT NULL auto_increment,

  hg_name varchar(200) default NULL,
  hg_alias varchar(200) default NULL,
  hg_notes varchar(255) default NULL,
  hg_notes_url varchar(255) default NULL,
  hg_action_url varchar(255) default NULL,
  hg_icon_image int default NULL,
  hg_map_icon_image int default NULL,
  hg_rrd_retention int default NULL,
  hg_comment text,
  hg_activate enum('0','1') NOT NULL default '1',

  PRIMARY KEY (hg_id),
  INDEX (hg_name),
  INDEX (hg_alias)
) ENGINE=InnoDB CHARACTER SET utf8;
