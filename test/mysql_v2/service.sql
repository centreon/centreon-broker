--
-- Service configuration table.
--
CREATE TABLE service (
  service_id int NOT NULL auto_increment,

  service_template_model_stm_id int default NULL,
  command_command_id int default NULL,
  timeperiod_tp_id int default NULL,
  command_command_id2 int default NULL,
  timeperiod_tp_id2 int default NULL,
  service_description varchar(200) default NULL,
  service_alias varchar(255) default NULL,
  display_name varchar(255) default NULL,
  service_is_volatile enum('0','1','2') default '2',
  service_max_check_attempts int default NULL,
  service_normal_check_interval int default NULL,
  service_retry_check_interval int default NULL,
  service_active_checks_enabled enum('0','1','2') default '2',
  service_passive_checks_enabled enum('0','1','2') default '2',
  initial_state enum('o','w','u','c') default NULL,
  service_parallelize_check enum('0','1','2') default '2',
  service_obsess_over_service enum('0','1','2') default '2',
  service_check_freshness enum('0','1','2') default '2',
  service_freshness_threshold int default NULL,
  service_event_handler_enabled enum('0','1','2') default '2',
  service_low_flap_threshold int default NULL,
  service_high_flap_threshold int default NULL,
  service_flap_detection_enabled enum('0','1','2') default '2',
  service_process_perf_data enum('0','1','2') default '2',
  service_retain_status_information enum('0','1','2') default '2',
  service_retain_nonstatus_information enum('0','1','2') default '2',
  service_notification_interval int default NULL,
  service_notification_options varchar(200) default NULL,
  service_notifications_enabled enum('0','1','2') default '2',
  contact_additive_inheritance boolean default 0,
  cg_additive_inheritance boolean default 0,
  service_inherit_contacts_from_host enum('0','1') default '1',
  service_first_notification_delay int default NULL,
  service_stalking_options varchar(200) default NULL,
  service_comment text,
  command_command_id_arg text,
  command_command_id_arg2 text,
  service_locked BOOLEAN default 0,
  service_register enum('0','1','2','3') NOT NULL default '0',
  service_activate enum('0','1') NOT NULL default '1',

  PRIMARY KEY (service_id),
  INDEX (service_template_model_stm_id),
  INDEX (command_command_id),
  INDEX (command_command_id2),
  INDEX (timeperiod_tp_id),
  INDEX (timeperiod_tp_id2),
  INDEX (service_description),
  FOREIGN KEY (command_command_id) REFERENCES command (command_id)
    ON DELETE SET NULL,
  FOREIGN KEY (command_command_id2) REFERENCES command (command_id)
    ON DELETE SET NULL,
  FOREIGN KEY (timeperiod_tp_id) REFERENCES timeperiod (tp_id)
    ON DELETE SET NULL,
  FOREIGN KEY (timeperiod_tp_id2) REFERENCES timeperiod (tp_id)
    ON DELETE SET NULL
) ENGINE=InnoDB CHARACTER SET utf8;
