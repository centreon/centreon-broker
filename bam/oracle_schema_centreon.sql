-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--    Business Activity Monitoring   --
-- ------------------------------------

-- meta_service
-- meta_service_relation
-- mod_bam
-- mod_bam_impacts
-- mod_bam_boolean
-- mod_bam_bool_rel
-- mod_bam_kpi
-- mod_bam_ba_tp_rel


--
-- Business Activities.
--
CREATE TABLE mod_bam (
  ba_id int NOT NULL,
  name varchar(254) default NULL,

  description varchar(254) default NULL,
  level_w float default NULL,
  level_c float default NULL,
  current_level float default NULL,
  downtime float default NULL,
  acknowledged float default NULL,
  activate enum('1','0') NOT NULL default '0',
  last_state_change int default NULL,
  current_status char(1) default NULL,
  in_downtime boolean default NULL,

  PRIMARY KEY (ba_id),
  UNIQUE (name)
);
CREATE SEQUENCE mod_bam_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_trigger
BEFORE INSERT ON mod_bam
FOR EACH ROW
BEGIN
  SELECT mod_bam_seq.nextval INTO :NEW.ba_id FROM dual;
END;
/

--
-- Impacts of KPI / boolean expressions.
--
CREATE TABLE mod_bam_impacts (
  id_impact int NOT NULL,
  impact float NOT NULL,

  PRIMARY KEY (id_impact)
);
CREATE SEQUENCE mod_bam_impacts_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_impacts_trigger
BEFORE INSERT ON mod_bam_impacts
FOR EACH ROW
BEGIN
  SELECT mod_bam_impacts_seq.nextval INTO :NEW.id_impact FROM dual;
END;
/

--
-- BAM boolean expressions.
--
CREATE TABLE mod_bam_boolean (
  boolean_id int NOT NULL,
  name varchar(255) NOT NULL,

  config_type smallint NOT NULL,
  impact float default NULL,
  impact_id int default NULL,
  expression text NOT NULL,
  bool_state boolean NOT NULL default 1,
  current_state boolean default NULL,
  activate boolean NOT NULL default 0,

  PRIMARY KEY (boolean_id),
  FOREIGN KEY (impact_id) REFERENCES mod_bam_impacts (id_impact)
    ON DELETE RESTRICT
);
CREATE SEQUENCE mod_bam_boolean_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_boolean_trigger
BEFORE INSERT ON mod_bam_boolean
FOR EACH ROW
BEGIN
  SELECT mod_bam_boolean_seq.nextval INTO :NEW.boolean_id FROM dual;
END;
/

--
-- Relations between BA and boolean expressions.
--
CREATE TABLE mod_bam_bool_rel (
  ba_id int NOT NULL,
  boolean_id int NOT NULL,

  FOREIGN KEY (ba_id) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (boolean_id) REFERENCES mod_bam_boolean (boolean_id)
    ON DELETE CASCADE
);

--
-- Key Performance Indicators.
--
CREATE TABLE mod_bam_kpi (
  kpi_id int NOT NULL,

  state_type enum('0','1') default NULL,
  kpi_type enum('0','1','2','3') NOT NULL default '0',
  host_id int default NULL,
  service_id int default NULL,
  id_indicator_ba int default NULL,
  id_ba int default NULL,
  meta_id int default NULL,
  boolean_id int default NULL,
  current_status smallint default NULL,
  last_level float default NULL,
  downtime float default NULL,
  acknowledged float default NULL,
  config_type enum('0', '1'),
  drop_warning float default NULL,
  drop_warning_impact_id int default NULL,
  drop_critical float default NULL,
  drop_critical_impact_id int default NULL,
  drop_unknown float default NULL,
  drop_unknown_impact_id int default NULL,
  activate enum('0','1') default '0',
  ignore_downtime enum('0','1') default '0',
  ignore_acknowledged enum('0','1') default '0',
  last_state_change int default NULL,
  in_downtime boolean default NULL,

  PRIMARY KEY (kpi_id),
  FOREIGN KEY (id_indicator_ba) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (id_ba) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (drop_warning_impact_id) REFERENCES mod_bam_impacts (id_impact)
    ON DELETE RESTRICT,
  FOREIGN KEY (drop_critical_impact_id) REFERENCES mod_bam_impacts (id_impact)
    ON DELETE RESTRICT,
  FOREIGN KEY (drop_unknown_impact_id) REFERENCES mod_bam_impacts (id_impact)
    ON DELETE RESTRICT
);
CREATE SEQUENCE mod_bam_kpi_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER mod_bam_kpi_trigger
BEFORE INSERT ON mod_bam_kpi
FOR EACH ROW
BEGIN
  SELECT mod_bam_kpi_seq.nextval INTO :NEW.kpi_id FROM dual;
END;
/

--
-- BA / Timeperiod relations.
--
CREATE TABLE mod_bam_ba_tp_rel (
  ba_id int NOT NULL,
  timeperiod_id int NOT NULL,
  is_default boolean NOT NULL default 0,

  FOREIGN KEY (ba_id) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE
);

--
-- Meta Services.
--
CREATE TABLE meta_service (
  meta_id int NOT NULL,

  meta_name varchar(254) default NULL,
  meta_display varchar(254) default NULL,
  check_period int default NULL,
  max_check_attempts int default NULL,
  normal_check_interval int default NULL,
  retry_check_interval int default NULL,
  notification_interval int default NULL,
  notification_period int default NULL,
  notification_options varchar(255) default NULL,
  notifications_enabled enum('0','1','2') default NULL,
  calcul_type enum('SOM','AVE','MIN','MAX') default NULL,
  data_source_type smallint NOT NULL default 0,
  meta_select_mode enum('1','2') default '1',
  regexp_str varchar(254) default NULL,
  metric varchar(255) default NULL,
  warning varchar(254) default NULL,
  critical varchar(254) default NULL,
  graph_id int default NULL,
  meta_comment text,
  meta_activate enum('0','1') default NULL,
  value float default NULL,

  PRIMARY KEY (meta_id)
);
CREATE SEQUENCE meta_service_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER meta_service_trigger
BEFORE INSERT ON meta_service
FOR EACH ROW
BEGIN
  SELECT meta_service_seq.nextval INTO :NEW.meta_id FROM dual;
END;
/

--
-- Meta Services Relationships.
--
CREATE TABLE meta_service_relation (
  msr_id int NOT NULL,

  meta_id int default NULL,
  host_id int default NULL,
  metric_id int default NULL,
  msr_comment text,
  activate enum('0','1') default NULL,

  PRIMARY KEY (msr_id),
  FOREIGN KEY (meta_id) REFERENCES meta_service (meta_id) ON DELETE CASCADE
);
CREATE SEQUENCE meta_service_relation_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER meta_service_relation_trigger
BEFORE INSERT ON meta_service_relation
FOR EACH ROW
BEGIN
  SELECT meta_service_relation_seq.nextval INTO :NEW.msr_id FROM dual;
END;
/
