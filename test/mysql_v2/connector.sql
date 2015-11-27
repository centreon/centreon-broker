--
-- Connector configuration table.
--
CREATE TABLE connector (
  id int unsigned NOT NULL auto_increment,

  name varchar(255) NOT NULL,
  description varchar(255) default NULL,
  command_line varchar(512) NOT NULL,
  enabled int unsigned NOT NULL default '1',
  created int unsigned NOT NULL,
  modified int unsigned NOT NULL,

  PRIMARY KEY (id),
  UNIQUE KEY (name),
  INDEX (enabled)
) ENGINE=InnoDB CHARACTER SET utf8;
