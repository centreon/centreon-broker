-- ------------------------------------
--                                   --
-- Centreon Broker's database schema --
--                                   --
--                                   --
--    Business Activity Monitoring   --
-- ------------------------------------

-- cfg_meta_service
-- cfg_meta_service_relation
-- cfg_bam
-- cfg_bam_ba_groups
-- cfg_bam_bagroup_ba_relation
-- cfg_bam_impacts
-- cfg_bam_boolean
-- cfg_bam_kpi
-- cfg_bam_poller_relations
-- cfg_bam_relations_ba_timeperiods


--
-- Business Activities.
--
CREATE TABLE cfg_bam (
  ba_id int NOT NULL,
  name varchar(254) default NULL,

  description varchar(254) default NULL,
  level_w float default NULL,
  level_c float default NULL,
  sla_month_percent_warn float default NULL,
  sla_month_percent_crit float default NULL,
  sla_month_duration_warn int default NULL,
  sla_month_duration_crit int default NULL,
  current_level float default NULL,
  downtime float default NULL,
  acknowledged float default NULL,
  activate enum('1','0') NOT NULL default '0',
  last_state_change int default NULL,
  current_status char(1) default NULL,
  in_downtime boolean default NULL,
  must_be_rebuild enum('0', '1', '2') NOT NULL default '0',
  id_reporting_period int default NULL,

  PRIMARY KEY (ba_id),
  UNIQUE (name),
  FOREIGN KEY (id_reporting_period) REFERENCES timeperiod (tp_id)
    ON DELETE SET NULL
);
CREATE SEQUENCE cfg_bam_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_trigger
BEFORE INSERT ON cfg_bam
FOR EACH ROW
BEGIN
  SELECT cfg_bam_seq.nextval INTO :NEW.ba_id FROM dual;
END;
/

--
-- BA / poller association.
--
CREATE TABLE cfg_bam_poller_relations (
  ba_id int NOT NULL,
  poller_id int NOT NULL
);

--
-- Impacts of KPI / boolean expressions.
--
CREATE TABLE cfg_bam_impacts (
  id_impact int NOT NULL,
  impact float NOT NULL,

  PRIMARY KEY (id_impact)
);
CREATE SEQUENCE cfg_bam_impacts_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_impacts_trigger
BEFORE INSERT ON cfg_bam_impacts
FOR EACH ROW
BEGIN
  SELECT cfg_bam_impacts_seq.nextval INTO :NEW.id_impact FROM dual;
END;
/

--
-- BAM boolean expressions.
--
CREATE TABLE cfg_bam_boolean (
  boolean_id int NOT NULL,
  name varchar(255) NOT NULL,

  expression text NOT NULL,
  bool_state boolean NOT NULL default 1,
  activate boolean NOT NULL default 0,

  PRIMARY KEY (boolean_id),
  UNIQUE (name)
);
CREATE SEQUENCE cfg_bam_boolean_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_boolean_trigger
BEFORE INSERT ON cfg_bam_boolean
FOR EACH ROW
BEGIN
  SELECT cfg_bam_boolean_seq.nextval INTO :NEW.boolean_id FROM dual;
END;
/

--
-- Key Performance Indicators.
--
CREATE TABLE cfg_bam_kpi (
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
  last_impact float default NULL,

  PRIMARY KEY (kpi_id),
  FOREIGN KEY (id_indicator_ba) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (id_ba) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (drop_warning_impact_id) REFERENCES cfg_bam_impacts (id_impact)
    ON DELETE RESTRICT,
  FOREIGN KEY (drop_critical_impact_id) REFERENCES cfg_bam_impacts (id_impact)
    ON DELETE RESTRICT,
  FOREIGN KEY (drop_unknown_impact_id) REFERENCES cfg_bam_impacts (id_impact)
    ON DELETE RESTRICT
);
CREATE SEQUENCE cfg_bam_kpi_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_kpi_trigger
BEFORE INSERT ON cfg_bam_kpi
FOR EACH ROW
BEGIN
  SELECT cfg_bam_kpi_seq.nextval INTO :NEW.kpi_id FROM dual;
END;
/

--
-- BA / Timeperiod relations.
--
CREATE TABLE cfg_bam_relations_ba_timeperiods (
  ba_id int NOT NULL,
  tp_id int NOT NULL,

  UNIQUE (ba_id, tp_id),
  FOREIGN KEY (ba_id) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (tp_id) REFERENCES timeperiod (tp_id)
    ON DELETE CASCADE
);

--
-- BA Groups (aka BV).
--
CREATE TABLE cfg_bam_ba_groups (
  id_ba_group int NOT NULL,
  ba_group_name varchar(255) NOT NULL,

  ba_group_description varchar(255) default NULL,
  visible enum('0', '1') NOT NULL default '1',

  PRIMARY KEY (id_ba_group),
  UNIQUE (ba_group_name)
);
CREATE SEQUENCE cfg_bam_ba_groups_seq
START WITH 1
INCREMENT BY 1;
CREATE TRIGGER cfg_bam_ba_groups_trigger
BEFORE INSERT ON cfg_bam_ba_groups
FOR EACH ROW
BEGIN
  SELECT cfg_bam_ba_groups_seq.nextval INTO :NEW.id_ba_group FROM dual;
END;
/

--
-- BA / Group relations.
--
CREATE TABLE cfg_bam_bagroup_ba_relation (
  id_ba int NOT NULL,
  id_ba_group int NOT NULL,

  UNIQUE (id_ba, id_ba_group),
  FOREIGN KEY (id_ba) REFERENCES cfg_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (id_ba_group) REFERENCES cfg_bam_ba_groups (id_ba_group)
    ON DELETE CASCADE
);

--
-- Meta Services.
--
CREATE TABLE cfg_meta_services (
  meta_id int NOT NULL,
  meta_name varchar(254) NOT NULL,

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

  PRIMARY KEY (meta_id),
  UNIQUE (meta_name)
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
CREATE TABLE cfg_meta_services_relations (
  meta_id int NOT NULL,
  host_id int NOT NULL,
  metric_id int NOT NULL,
  msr_comment text,
  activate enum('0','1') default NULL,

  UNIQUE (meta_id, host_id, metric_id),
  FOREIGN KEY (meta_id) REFERENCES cfg_meta_services (meta_id) ON DELETE CASCADE
);
