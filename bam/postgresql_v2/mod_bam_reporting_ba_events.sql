--
-- BA events.
--
CREATE TABLE mod_bam_reporting_ba_events (
  ba_event_id serial,
  ba_id int NOT NULL,
  start_time int NOT NULL,

  first_level double default NULL,
  end_time int default NULL,
  status smallint default NULL,
  in_downtime boolean default NULL,

  PRIMARY KEY (ba_event_id),
  KEY (ba_id, start_time),
  KEY (ba_id, end_time)
);
