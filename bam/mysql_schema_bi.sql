-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--    Business Activity Reporting    --
-- ------------------------------------

-- mod_bam_reporting_ba
-- mod_bam_reporting_ba_availabilities
-- mod_bam_reporting_ba_events
-- mod_bam_reporting_ba_events_durations
-- mod_bam_reporting_bv
-- mod_bam_reporting_kpi
-- mod_bam_reporting_kpi_events
-- mod_bam_reporting_relations_ba_bv
-- mod_bam_reporting_relations_ba_kpi_events
-- mod_bam_reporting_relations_ba_timeperiods
-- mod_bam_reporting_timeperiods
-- mod_bam_reporting_timeperiods_exceptions
-- mod_bam_reporting_timeperiods_exclusions


--
-- Business Views.
--
CREATE TABLE mod_bam_reporting_bv (
  bv_id int NOT NULL auto_increment,
  bv_name varchar(45) default NULL,

  bv_description text default NULL,

  PRIMARY KEY (bv_id),
  UNIQUE (bv_name)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Business Activities.
--
CREATE TABLE mod_bam_reporting_ba (
  ba_id int NOT NULL,
  ba_name varchar(45) default NULL,

  ba_description text default NULL,
  sla_month_percent_crit float default NULL,
  sla_month_percent_warn float default NULL,
  sla_month_duration_crit int default NULL,
  sla_month_duration_warn int default NULL,

  PRIMARY KEY (ba_id),
  UNIQUE (ba_name)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Key Performance Indicators.
--
CREATE TABLE mod_bam_reporting_kpi (
  kpi_id int NOT NULL,
  kpi_name varchar(45) default NULL,

  ba_id int default NULL,
  ba_name varchar(45) default NULL,
  host_id int default NULL,
  host_name varchar(45) default NULL,
  service_id int default NULL,
  service_description varchar(45) default NULL,
  kpi_ba_id int default NULL,
  kpi_ba_name varchar(45) default NULL,
  meta_service_id int default NULL,
  meta_service_name varchar(45),
  boolean_id int default NULL,
  boolean_name varchar(45),
  impact_warning float default NULL,
  impact_critical float default NULL,
  impact_unknown float default NULL,

  PRIMARY KEY (kpi_id),
  FOREIGN KEY (ba_id) REFERENCES mod_bam_reporting_ba (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (kpi_ba_id) REFERENCES mod_bam_reporting_ba (ba_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Relations between BA and BV.
--
CREATE TABLE mod_bam_reporting_relations_ba_bv (
  ba_bv_id int NOT NULL auto_increment,
  bv_id int NOT NULL,
  ba_id int NOT NULL,

  PRIMARY KEY (ba_bv_id),
  FOREIGN KEY (bv_id) REFERENCES mod_bam_reporting_bv (bv_id)
    ON DELETE CASCADE,
  FOREIGN KEY (ba_id) REFERENCES mod_bam_reporting_ba (ba_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

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
  KEY (ba_id, start_time)
) ENGINE=InnoDB CHARACTER SET utf8;

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

--
-- Relations between BA events and KPI events.
--
CREATE TABLE mod_bam_reporting_relations_ba_kpi_events (
  ba_event_id int NOT NULL,
  kpi_event_id int NOT NULL,

  FOREIGN KEY (ba_event_id) REFERENCES mod_bam_reporting_ba_events (ba_event_id)
    ON DELETE CASCADE,
  FOREIGN KEY (kpi_event_id) REFERENCES mod_bam_reporting_kpi_events (kpi_event_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

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

--
-- Timeperiods exceptions.
--
CREATE TABLE mod_bam_reporting_timeperiods_exceptions (
  timeperiod_id int NOT NULL,
  daterange varchar(255) NOT NULL,
  timerange varchar(255) NOT NULL,

  FOREIGN KEY (timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Timeperiods exclusions.
--
CREATE TABLE mod_bam_reporting_timeperiods_exclusions (
  timeperiod_id int NOT NULL,
  excluded_timeperiod_id int NOT NULL,

  FOREIGN KEY (timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE,
  FOREIGN KEY (excluded_timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- BA/timeperiods relations.
--
CREATE TABLE mod_bam_reporting_relations_ba_timeperiods (
  ba_id int default NULL,
  timeperiod_id int default NULL,
  is_default boolean default NULL,

  FOREIGN KEY (ba_id) REFERENCES mod_bam_reporting_ba (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- BA events durations.
--
CREATE TABLE mod_bam_reporting_ba_events_durations (
  ba_event_id int NOT NULL,
  timeperiod_id int NOT NULL,

  start_time int default NULL,
  end_time int default NULL,
  duration int default NULL,
  sla_duration int default NULL,
  timeperiod_is_default boolean default NULL,

  UNIQUE (ba_event_id, timeperiod_id),
  FOREIGN KEY (ba_event_id) REFERENCES mod_bam_reporting_ba_events (ba_event_id)
    ON DELETE CASCADE,
  KEY (end_time, start_time)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- BA availabilities.
--
CREATE TABLE mod_bam_reporting_ba_availabilities (
  ba_id int NOT NULL,
  time_id int NOT NULL,
  timeperiod_id int NOT NULL,

  available int default NULL,
  unavailable int default NULL,
  degraded int default NULL,
  unknown int default NULL,
  downtime int default NULL,
  alert_unavailable_opened int default NULL,
  alert_degraded_opened int default NULL,
  alert_unknown_opened int default NULL,
  nb_downtime int default NULL,
  timeperiod_is_default boolean default NULL,

  UNIQUE (ba_id, time_id, timeperiod_id)
) ENGINE=InnoDB CHARACTER SET utf8;
