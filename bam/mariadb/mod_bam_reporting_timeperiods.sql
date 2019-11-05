--
-- Timeperiods.
--
CREATE TABLE mod_bam_reporting_timeperiods (
  timeperiod_id int NOT NULL,
  name varchar(200) default NULL,
  sunday varchar(200) default NULL,
  monday varchar(200) default NULL,
  tuesday varchar(200) default NULL,
  wednesday varchar(200) default NULL,
  thursday varchar(200) default NULL,
  friday varchar(200) default NULL,
  saturday varchar(200) default NULL,

  PRIMARY KEY (timeperiod_id)
) ENGINE=InnoDB CHARACTER SET utf8;
