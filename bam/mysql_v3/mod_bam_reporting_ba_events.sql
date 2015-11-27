--
-- BA events.
--
CREATE TABLE mod_bam_reporting_ba_events (
  ba_event_id int NOT NULL auto_increment,
  ba_id int NOT NULL,
  start_time int NOT NULL,

  first_level double default NULL,
  end_time int default NULL,
  status tinyint default NULL,
  in_downtime boolean default NULL,

  PRIMARY KEY (ba_event_id),
  UNIQUE (ba_id, start_time),
  KEY (ba_id, end_time)
) ENGINE=InnoDB CHARACTER SET utf8;
