-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--    Business Activity Reporting    --
-- ------------------------------------

-- ba
-- ba_availabilities
-- ba_events
-- ba_events_durations
-- bv
-- kpi
-- kpi_events
-- relations_ba_bv
-- relations_ba_kpi_events


--
-- Business Views.
--
CREATE TABLE bv (
  bv_id int NOT NULL,
  bv_name varchar(45) default NULL,

  bv_description text default NULL,

  PRIMARY KEY (bv_id),
  UNIQUE (bv_name)
);
CREATE SEQUENCE bv_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER bv_trigger
BEFORE INSERT ON bv
FOR EACH ROW
BEGIN
  SELECT bv_seq.nextval INTO :NEW.bv_id FROM dual;
END;
/

--
-- Business Activities.
--
CREATE TABLE ba (
  ba_id int NOT NULL,
  ba_name varchar(45) default NULL,

  ba_description text default NULL,
  sla_month_percent_1 float default NULL,
  sla_month_percent_2 float default NULL,
  sla_month_duration_1 int default NULL,
  sla_month_duration_2 int default NULL,

  PRIMARY KEY (ba_id),
  UNIQUE (ba_name)
);
CREATE SEQUENCE ba_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER ba_trigger
BEFORE INSERT ON ba
FOR EACH ROW
BEGIN
  SELECT ba_seq.nextval INTO :NEW.ba_id FROM dual;
END;
/

--
-- Key Performance Indicators.
--
CREATE TABLE kpi (
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
  UNIQUE (kpi_name),
  FOREIGN KEY (ba_id) REFERENCES ba (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (kpi_ba_id) REFERENCES ba (ba_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE kpi_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER kpi_trigger
BEFORE INSERT ON kpi
FOR EACH ROW
BEGIN
  SELECT kpi_seq.nextval INTO :NEW.kpi_id FROM dual;
END;
/

--
-- Relations between BA and BV.
--
CREATE TABLE relations_ba_bv (
  ba_bv_id int NOT NULL,
  bv_id int NOT NULL,
  ba_id int NOT NULL,

  PRIMARY KEY (ba_bv_id),
  FOREIGN KEY (bv_id) REFERENCES bv (bv_id)
    ON DELETE CASCADE,
  FOREIGN KEY (ba_id) REFERENCES ba (ba_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE relations_ba_bv_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER relations_ba_bv_trigger
BEFORE INSERT ON relations_ba_bv
FOR EACH ROW
BEGIN
  SELECT relations_ba_bv_seq.nextval INTO :NEW.ba_bv_id FROM dual;
END;
/

--
-- BA events.
--
CREATE TABLE ba_events (
  ba_event_id int NOT NULL,
  ba_id int NOT NULL,
  start_time int NOT NULL,

  end_time int default NULL,
  status char(1) default NULL,
  in_downtime boolean default NULL,

  PRIMARY KEY (ba_event_id),
  FOREIGN KEY (ba_id) REFERENCES ba (ba_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE ba_events_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER ba_events_trigger
BEFORE INSERT ON ba_events
FOR EACH ROW
BEGIN
  SELECT ba_events_seq.nextval INTO :NEW.ba_events_id FROM dual;
END;
/

--
-- KPI events.
--
CREATE TABLE kpi_events (
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
  FOREIGN KEY (kpi_id) REFERENCES kpi (kpi_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE kpi_events_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER kpi_events_trigger
BEFORE INSERT ON kpi_events
FOR EACH ROW
BEGIN
  SELECT kpi_events_seq.nextval INTO :NEW.kpi_event_id FROM dual;
END;
/

--
-- Relations between BA events and KPI events.
--
CREATE TABLE relations_ba_kpi_events (
  ba_kpi_event_id int NOT NULL,
  ba_event_id int NOT NULL,
  kpi_event_id int NOT NULL,

  PRIMARY KEY (ba_kpi_event_id),
  FOREIGN KEY (ba_event_id) REFERENCES ba_events (ba_event_id)
    ON DELETE CASCADE,
  FOREIGN KEY (kpi_event_id) REFERENCES kpi_events (kpi_event_id)
    ON DELETE CASCADE
);
CREATE SEQUENCE relations_ba_kpi_events_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER relations_ba_kpi_events_trigger
BEFORE INSERT ON relations_ba_kpi_events
FOR EACH ROW
BEGIN
  SELECT relations_ba_kpi_events_seq.nextval INTO :NEW.ba_kpi_event_id FROM dual;
END;
/

--
-- BA events durations.
--
CREATE TABLE ba_events_durations (
  ba_event_id int NOT NULL,
  timeperiod_id int NOT NULL,

  start_time int default NULL,
  end_time int default NULL,
  duration int default NULL,
  sla_duration int default NULL,
  timeperiod_is_default boolean default NULL,

  UNIQUE (ba_event_id, timeperiod_id),
  FOREIGN KEY (ba_event_id) REFERENCES ba_events (ba_event_id)
    ON DELETE CASCADE
);

--
-- BA availabilities.
--
CREATE TABLE ba_availabilities (
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
  alert_downtime_opened int default NULL,
  timeperiod_is_default boolean default NULL,

  UNIQUE (ba_id, time_id, timeperiod_id),
  FOREIGN KEY (ba_id) REFERENCES ba (ba_id)
    ON DELETE CASCADE
);
