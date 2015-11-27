--
-- KPI events.
--
CREATE TABLE mod_bam_reporting_kpi_events (
  kpi_event_id int NOT NULL,
  kpi_id int NOT NULL,
  start_time int NOT NULL,

  end_time int default NULL,
  status char(1) default NULL,
  in_downtime boolean default NULL,
  impact_level char(1) default NULL,
  first_output text default NULL,
  first_perfdata varchar(45) default NULL,

  PRIMARY KEY (kpi_event_id),
  UNIQUE (kpi_id, start_time)
);
CREATE SEQUENCE mod_bam_reporting_kpi_events_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_reporting_kpi_events_trigger
BEFORE INSERT ON mod_bam_reporting_kpi_events
FOR EACH ROW
BEGIN
  SELECT mod_bam_reporting_kpi_events_seq.nextval INTO :NEW.kpi_event_id FROM dual;
END;
/
