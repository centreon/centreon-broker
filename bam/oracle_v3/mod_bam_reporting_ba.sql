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
);
