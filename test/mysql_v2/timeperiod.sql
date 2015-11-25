--
-- Timeperiod configuration table.
--
CREATE TABLE timeperiod (
  tp_id int NOT NULL auto_increment,

  tp_name varchar(200) default NULL,
  tp_alias varchar(200) default NULL,
  tp_sunday varchar(200) default NULL,
  tp_monday varchar(200) default NULL,
  tp_tuesday varchar(200) default NULL,
  tp_wednesday varchar(200) default NULL,
  tp_thursday varchar(200) default NULL,
  tp_friday varchar(200) default NULL,
  tp_saturday varchar(200) default NULL,

  PRIMARY KEY (tp_id)
) ENGINE=InnoDB CHARACTER SET utf8;
