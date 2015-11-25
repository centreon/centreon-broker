--
-- Configuration of exclusions of timeperiods.
--
CREATE TABLE timeperiod_exclude_relations (
  exclude_id int NOT NULL auto_increment,

  timeperiod_id int NOT NULL,
  timeperiod_exclude_id int NOT NULL,

  PRIMARY KEY (exclude_id)
) ENGINE=InnoDB CHARACTER SET utf8;
