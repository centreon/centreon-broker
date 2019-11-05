--
-- KPI events.
--
CREATE TABLE mod_bam_reporting_kpi_events (
  kpi_event_id int NOT NULL auto_increment,
  kpi_id int NOT NULL,
  start_time int NOT NULL,

  end_time int default NULL,
  status tinyint default NULL,
  in_downtime boolean default NULL,
  impact_level tinyint default NULL,
  first_output text default NULL,
  first_perfdata varchar(45) default NULL,

  PRIMARY KEY (kpi_event_id),
  KEY (kpi_id, start_time)
) ENGINE=InnoDB CHARACTER SET utf8;
