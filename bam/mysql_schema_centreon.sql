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


--
-- Business Activities.
--
CREATE TABLE mod_bam (
  ba_id int NOT NULL auto_increment,
  name varchar(254) default NULL,

  description varchar(254) default NULL,
  level_w float default NULL,
  level_c float default NULL,
  current_level float default NULL,
  downtime float default NULL,
  acknowledged float default NULL,
  activate enum('1','0') NOT NULL default '0',

  PRIMARY KEY (ba_id),
  UNIQUE (name)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Impacts of KPI / boolean expressions.
--
CREATE TABLE mod_bam_impacts (
  id_impact int NOT NULL auto_increment,
  impact float NOT NULL,

  PRIMARY KEY (id_impact)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- BAM boolean expressions.
--
CREATE TABLE mod_bam_boolean (
  boolean_id int NOT NULL auto_increment,
  name varchar(255) NOT NULL,

  config_type tinyint NOT NULL,
  impact float default NULL,
  impact_id int default NULL,
  expression text NOT NULL,
  bool_state boolean NOT NULL default 1,
  current_state boolean default NULL,
  activate boolean NOT NULL default 0,

  PRIMARY KEY (boolean_id),
  FOREIGN KEY (impact_id) REFERENCES mod_bam_impacts (id_impact)
    ON DELETE RESTRICT
) ENGINE=InnoDB CHARACTER SET utf8;

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
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Key Performance Indicators.
--
CREATE TABLE mod_bam_kpi (
  kpi_id int NOT NULL auto_increment,

  state_type enum('0','1') default NULL,
  kpi_type enum('0','1','2') NOT NULL default '0',
  host_id int default NULL,
  service_id int default NULL,
  id_indicator_ba int default NULL,
  id_ba int default NULL,
  meta_id int default NULL,
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
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Meta Services.
--
CREATE TABLE meta_service (
  meta_id int NOT NULL auto_increment,

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
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Meta Services Relationships.
--
CREATE TABLE meta_service_relation (
  msr_id int NOT NULL auto_increment,

  meta_id int default NULL,
  host_id int default NULL,
  metric_id int default NULL,
  msr_comment text,
  activate enum('0','1') default NULL,

  PRIMARY KEY (msr_id),
  FOREIGN KEY (meta_id) REFERENCES meta_service (meta_id) ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
