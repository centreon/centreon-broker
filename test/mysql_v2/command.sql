--
-- Command configuration table.
--
CREATE TABLE command (
  command_id int NOT NULL auto_increment,

  connector_id int unsigned default NULL,
  command_name varchar(200) default NULL,
  command_line text,
  command_example varchar(254) default NULL,
  command_type tinyint default NULL,
  enable_shell int unsigned NOT NULL default '0',
  command_comment text,
  graph_id int default NULL,
  cmd_cat_id int default NULL,

  PRIMARY KEY (command_id),
  INDEX (connector_id),
  FOREIGN KEY (connector_id) REFERENCES connector (id)
    ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
