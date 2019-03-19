--
-- Business Activities.
--
CREATE TABLE cfg_bam (
  ba_id int NOT NULL auto_increment,
  name varchar(254) default NULL,
  state_source int default NULL,

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
  current_status tinyint default NULL,
  in_downtime boolean default NULL,
  must_be_rebuild enum('0', '1', '2') NOT NULL default '0',
  id_reporting_period int default NULL,
  ba_type_id int NOT NULL,
  organization_id int NOT NULL,

  PRIMARY KEY (ba_id),
  UNIQUE (name),
  FOREIGN KEY (id_reporting_period) REFERENCES cfg_timeperiods (tp_id)
    ON DELETE SET NULL,
  FOREIGN KEY (ba_type_id) REFERENCES cfg_bam_ba_types (ba_type_id)
    ON DELETE RESTRICT,
  FOREIGN KEY (organization_id) REFERENCES cfg_organizations (organization_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;
