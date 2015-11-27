--
-- BA events.
--
CREATE TABLE mod_bam_reporting_ba_events (
  ba_event_id int NOT NULL,
  ba_id int NOT NULL,
  start_time int NOT NULL,

  first_level double default NULL,
  end_time int default NULL,
  status char(1) default NULL,
  in_downtime boolean default NULL,

  PRIMARY KEY (ba_event_id),
  UNIQUE (ba_id, start_time),
  KEY (ba_id, end_time)
);
CREATE SEQUENCE mod_bam_reporting_ba_events_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_reporting_ba_events_trigger
BEFORE INSERT ON mod_bam_reporting_ba_events
FOR EACH ROW
BEGIN
  SELECT mod_bam_reporting_ba_events_seq.nextval INTO :NEW.ba_events_id FROM dual;
END;
/
