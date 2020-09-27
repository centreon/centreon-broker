--
-- Structure de la table `mod_bam`
--

CREATE TABLE IF NOT EXISTS `mod_bam` (
  `ba_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(254) DEFAULT NULL,
  `state_source` int NOT NULL DEFAULT '0',
  `description` varchar(254) DEFAULT NULL,
  `infrastructure_view` varchar(254) DEFAULT NULL,
  `level_w` float DEFAULT NULL,
  `level_c` float DEFAULT NULL,
  `sla_month_percent_warn` float DEFAULT NULL,
  `sla_month_percent_crit` float DEFAULT NULL,
  `sla_month_duration_warn` int DEFAULT NULL,
  `sla_month_duration_crit` int DEFAULT NULL,
  `id_notification_period` int(11) DEFAULT NULL,
  `id_reporting_period` int(11) DEFAULT NULL,
  `notification_interval` int(11) DEFAULT NULL,
  `notification_options` varchar(255) DEFAULT NULL,
  `notifications_enabled` enum('0','1','2') DEFAULT NULL,
  `first_notification_delay` int(11) DEFAULT NULL,
  `recovery_notification_delay` int(11) DEFAULT NULL,
  `max_check_attempts` int(11) DEFAULT NULL,
  `normal_check_interval` int(11) DEFAULT NULL,
  `retry_check_interval` int(11) DEFAULT NULL,
  `current_level` float DEFAULT NULL,
  `calculate` enum('0','1') NOT NULL DEFAULT '0',
  `downtime` float NOT NULL DEFAULT '0',
  `acknowledged` float NOT NULL DEFAULT '0',
  `must_be_rebuild` enum('0','1','2') DEFAULT '0',
  `last_state_change` int default NULL,
  `current_status` tinyint default NULL,
  `in_downtime` BOOLEAN default NULL,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `graph_id` int(11) DEFAULT NULL,
  `icon_id` int(11) DEFAULT NULL,
  `geo_coords` varchar(32) DEFAULT NULL,
  `event_handler_enabled` enum('0', '1') DEFAULT NULL,
  `event_handler_command` int(11) DEFAULT NULL,
  `event_handler_args` varchar(254) DEFAULT NULL,
  `graph_style` varchar(254) DEFAULT NULL,
  `activate` enum('1','0') DEFAULT NULL,
  `inherit_kpi_downtimes` BOOLEAN default 0,
  `comment` text,
  PRIMARY KEY (`ba_id`),
  KEY `name_index` (`name`),
  KEY `description_index` (`description`),
  KEY `calculate_index` (`calculate`),
  KEY `currentlevel_index` (`current_level`),
  KEY `levelw_index` (`level_w`),
  KEY `levelc_index` (`level_c`),
  KEY `id_notification_period` (`id_notification_period`),
  KEY `id_reporting_period` (`id_reporting_period`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `icon_index` (`icon_id`),
  KEY `graph_index` (`graph_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

--
-- Contraintes pour les tables export�es
--

--
-- Contraintes pour la table `mod_bam`
--
ALTER TABLE `mod_bam`
  ADD CONSTRAINT `mod_bam_ibfk_1` FOREIGN KEY (`id_notification_period`) REFERENCES `timeperiod` (`tp_id`) ON DELETE SET NULL,
  ADD CONSTRAINT `mod_bam_ibfk_5` FOREIGN KEY (`id_reporting_period`) REFERENCES `timeperiod` (`tp_id`) ON DELETE SET NULL,
  ADD CONSTRAINT `mod_bam_ibfk_3` FOREIGN KEY (`graph_id`) REFERENCES `giv_graphs_template` (`graph_id`) ON DELETE SET NULL,
  ADD CONSTRAINT `mod_bam_ibfk_4` FOREIGN KEY (`icon_id`) REFERENCES `view_img` (`img_id`) ON DELETE SET NULL,
  ADD CONSTRAINT `mod_bam_ibfk_6` FOREIGN KEY (`event_handler_command`) REFERENCES `command` (`command_id`) ON DELETE SET NULL;

--
-- Structure de la table `mod_bam_relations_ba_timeperiods`
--
CREATE TABLE IF NOT EXISTS mod_bam_relations_ba_timeperiods (
	`ba_id` int(11) NOT NULL,
	`tp_id` int(11) NOT NULL,
	KEY `ba_id` (`ba_id`),
	KEY `tp_id` (`tp_id`),
	CONSTRAINT `mod_bam_relations_ba_timeperiods_ibfk_1` FOREIGN KEY (`ba_id`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
	CONSTRAINT `mod_bam_relations_ba_timeperiods_ibfk_2` FOREIGN KEY (`tp_id`) REFERENCES `timeperiod` (`tp_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Structure de la table `mod_bam_boolean`
--
CREATE TABLE  IF NOT EXISTS `mod_bam_boolean` (
    `boolean_id` INT( 11 ) NOT NULL AUTO_INCREMENT ,
    `name` VARCHAR( 255 ) NOT NULL ,
    `expression` TEXT NOT NULL ,
    `bool_state` BOOL NOT NULL DEFAULT '1',
    `comments` TEXT NULL ,
    `activate` TINYINT NOT NULL ,
    PRIMARY KEY (  `boolean_id` )
) ENGINE=INNODB CHARACTER SET utf8 COLLATE utf8_general_ci;

--
-- Structure de la table `mod_bam_kpi`
--
CREATE TABLE IF NOT EXISTS `mod_bam_kpi` (
  `kpi_id` int(11) NOT NULL AUTO_INCREMENT,  
  `state_type` enum('0','1') NOT NULL DEFAULT '1',
  `kpi_type` enum('0','1','2','3') NOT NULL DEFAULT '0',
  `host_id` int(11) DEFAULT NULL,
  `service_id` int(11) DEFAULT NULL,
  `id_indicator_ba` int(11) DEFAULT NULL,
  `id_ba` int(11) DEFAULT NULL,
  `meta_id` int(11) DEFAULT NULL,
  `boolean_id` int(11) DEFAULT NULL,
  `current_status` smallint(6) DEFAULT NULL,
  `last_level` float DEFAULT NULL,
  `last_impact` float DEFAULT NULL,
  `downtime` float DEFAULT NULL,
  `acknowledged` float DEFAULT NULL,
  `comment` text,
  `config_type` enum('0', '1'),
  `drop_warning` float DEFAULT NULL,
  `drop_warning_impact_id` int(11) DEFAULT NULL,
  `drop_critical` float DEFAULT NULL,
  `drop_critical_impact_id` int(11) DEFAULT NULL,
  `drop_unknown` float DEFAULT NULL,
  `drop_unknown_impact_id` int(11) DEFAULT NULL,
  `activate` enum('0','1') DEFAULT '1',
  `ignore_downtime` enum('0','1') DEFAULT '0',
  `ignore_acknowledged` enum('0','1') DEFAULT '0',
  `last_state_change` int default NULL,
  `in_downtime` boolean default NULL,
  `valid` BOOLEAN NOT NULL DEFAULT 1,
  PRIMARY KEY (`kpi_id`),
  KEY `ba_index` (`id_ba`),
  KEY `ba_indicator_index` (`id_indicator_ba`),
  KEY `host_id` (`host_id`),
  KEY `svc_id` (`service_id`),
  KEY `ms_index` (`meta_id`),
  KEY `boolean_id` (`boolean_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

--
-- Contraintes pour les tables export�es
--

--
-- Contraintes pour la table `mod_bam_kpi`
--
ALTER TABLE `mod_bam_kpi`
  ADD CONSTRAINT `mod_bam_kpi_ibfk_3` FOREIGN KEY (`host_id`) REFERENCES `host` (`host_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_kpi_ibfk_4` FOREIGN KEY (`service_id`) REFERENCES `service` (`service_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_kpi_ibfk_5` FOREIGN KEY (`id_indicator_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_kpi_ibfk_6` FOREIGN KEY (`id_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_kpi_ibfk_7` FOREIGN KEY (`meta_id`) REFERENCES `meta_service` (`meta_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_kpi_ibfk_8` FOREIGN KEY (`boolean_id`) REFERENCES `mod_bam_boolean` (`boolean_id`) ON DELETE CASCADE;

--
-- Structure de la table `mod_bam_impacts`
--
CREATE TABLE IF NOT EXISTS `mod_bam_impacts` (
  `id_impact` INT( 11 ) NOT NULL AUTO_INCREMENT PRIMARY KEY ,
  `code` TINYINT( 4 ) NOT NULL ,
  `impact` FLOAT NOT NULL,
  `color` VARCHAR( 7 ) default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Structure de la table `mod_bam_ba_groups`
-- 

CREATE TABLE IF NOT EXISTS `mod_bam_ba_groups` (
  `id_ba_group` int(11) NOT NULL AUTO_INCREMENT,
  `ba_group_name` varchar(255) default NULL,
  `ba_group_description` varchar(255) default NULL,
  `visible` enum('0','1') default NULL,
  PRIMARY KEY  (`id_ba_group`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Structure de la table `mod_bam_bagroup_ba_relation`
-- 

CREATE TABLE IF NOT EXISTS `mod_bam_bagroup_ba_relation` (
  `id_bgr` int(11) NOT NULL auto_increment,
  `id_ba` int(11) NOT NULL,
  `id_ba_group` int(11) NOT NULL,
  PRIMARY KEY  (`id_bgr`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_bagroup_ba_relation`
--
ALTER TABLE `mod_bam_bagroup_ba_relation`
  ADD CONSTRAINT `mod_bam_bagroup_ba_relation_ibfk_1` FOREIGN KEY (`id_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_bagroup_ba_relation_ibfk_2` FOREIGN KEY (`id_ba_group`) REFERENCES `mod_bam_ba_groups` (`id_ba_group`) ON DELETE CASCADE;

-- 
-- Structure de la table `mod_bam_acl`
-- 
CREATE TABLE IF NOT EXISTS `mod_bam_acl` (
  `acl_ba_id` int(11) NOT NULL auto_increment,
  `acl_group_id` int(11) NOT NULL,
  `ba_group_id` int(11) NOT NULL,
  PRIMARY KEY  (`acl_ba_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_acl`
--
ALTER TABLE `mod_bam_acl`
  ADD CONSTRAINT `mod_bam_acl_ibfk_1` FOREIGN KEY (`ba_group_id`) REFERENCES `mod_bam_ba_groups` (`id_ba_group`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_acl_ibfk_2` FOREIGN KEY (`acl_group_id`) REFERENCES `acl_groups` (`acl_group_id`) ON DELETE CASCADE;


-- 
-- Structure de la table `mod_bam_cg_relation`
-- 

CREATE TABLE IF NOT EXISTS `mod_bam_cg_relation` (
  `id_cgba_r` int(11) NOT NULL auto_increment,
  `id_ba` int(11) NOT NULL,
  `id_cg` int(11) NOT NULL,
  PRIMARY KEY  (`id_cgba_r`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_cg_relation`
--
ALTER TABLE `mod_bam_cg_relation`
  ADD CONSTRAINT `mod_bam_cg_relation_ibfk_1` FOREIGN KEY (`id_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_cg_relation_ibfk_2` FOREIGN KEY (`id_cg`) REFERENCES `contactgroup` (`cg_id`) ON DELETE CASCADE;

-- 
-- Structure de la table `mod_bam_escal_relation`
-- 
CREATE TABLE IF NOT EXISTS `mod_bam_escal_relation` (
  `id_escbar` int(11) NOT NULL auto_increment,
  `id_ba` int(11) NOT NULL,
  `id_esc` int(11) NOT NULL,
  PRIMARY KEY  (`id_escbar`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_escal_relation`
--
ALTER TABLE `mod_bam_escal_relation`
  ADD CONSTRAINT `mod_bam_escal_relation_ibfk_1` FOREIGN KEY (`id_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_escal_relation_ibfk_2` FOREIGN KEY (`id_esc`) REFERENCES `escalation` (`esc_id`) ON DELETE CASCADE;

-- 
-- Structure de la table `mod_bam_dep_parent_relation`
-- 
CREATE TABLE IF NOT EXISTS `mod_bam_dep_parent_relation` (
  `id_dpr` int(11) NOT NULL auto_increment,
  `id_ba` int(11) NOT NULL,
  `id_dep` int(11) NOT NULL,
  PRIMARY KEY  (`id_dpr`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_dep_parent_relation`
--
ALTER TABLE `mod_bam_dep_parent_relation`
  ADD CONSTRAINT `mod_bam_dep_parent_relation_ibfk_1` FOREIGN KEY (`id_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_dep_parent_relation_ibfk_2` FOREIGN KEY (`id_dep`) REFERENCES `dependency` (`dep_id`) ON DELETE CASCADE;


-- 
-- Structure de la table `mod_bam_dep_child_relation`
-- 
CREATE TABLE IF NOT EXISTS `mod_bam_dep_child_relation` (
  `id_dcr` int(11) NOT NULL auto_increment,
  `id_ba` int(11) NOT NULL,
  `id_dep` int(11) NOT NULL,
  PRIMARY KEY  (`id_dcr`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_dep_child_relation`
--
ALTER TABLE `mod_bam_dep_parent_relation`
  ADD CONSTRAINT `mod_bam_dep_child_relation_ibfk_1` FOREIGN KEY (`id_ba`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_dep_child_relation_ibfk_2` FOREIGN KEY (`id_dep`) REFERENCES `dependency` (`dep_id`) ON DELETE CASCADE;

-- 
-- Structure de la table `mod_bam_user_preferences`
-- 

CREATE TABLE IF NOT EXISTS `mod_bam_user_preferences` (
  `user_id` int(11) NULL,
  `pref_key` varchar(255) character set utf8 NOT NULL,
  `pref_value` varchar(255) character set utf8 NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_user_preferences`
--
ALTER TABLE `mod_bam_user_preferences`
  ADD CONSTRAINT `mod_bam_user_pref_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `contact` (`contact_id`) ON DELETE CASCADE;


-- 
-- Structure de la table `mod_bam_user_overview_relation`
-- 
CREATE TABLE IF NOT EXISTS `mod_bam_user_overview_relation` (
  `user_id` int(11) NULL,
  `ba_id` int(11) NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Contraintes pour la table `mod_bam_user_overview_relation`
--
ALTER TABLE `mod_bam_user_overview_relation`
  ADD CONSTRAINT `mod_bam_user_overview_relation_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `contact` (`contact_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `mod_bam_user_overview_relation_ibfk_2` FOREIGN KEY (`ba_id`) REFERENCES `mod_bam` (`ba_id`) ON DELETE CASCADE;

--
-- Create table for relation between BA and Poller
--
CREATE TABLE mod_bam_poller_relations (
  ba_id int NOT NULL,
  poller_id int NOT NULL,
  FOREIGN KEY (ba_id) REFERENCES mod_bam (ba_id)
    ON DELETE CASCADE,
  FOREIGN KEY (poller_id) REFERENCES nagios_server (id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Create table for relation between contact and notification options
--
CREATE TABLE IF NOT EXISTS `mod_bam_contact_notification_options` (
  `contact_contact_id` INT(11) NOT NULL,
  `notification_options` VARCHAR(10) NOT NULL,
  INDEX `fk_mod_bam_notification_option_contact1_idx` (`contact_contact_id` ASC),
  PRIMARY KEY (`contact_contact_id`),
  CONSTRAINT `fk_mod_bam_notification_option_contact1`
    FOREIGN KEY (`contact_contact_id`)
      REFERENCES `contact` (`contact_id`)
      ON DELETE CASCADE
      ON UPDATE NO ACTION
) ENGINE = InnoDB DEFAULT CHARACTER SET = utf8;

--
-- Create relation between a contact and a command
--
CREATE TABLE IF NOT EXISTS `mod_bam_contact_command` (
  `contact_contact_id` INT(11) NOT NULL,
  `command_command_id` INT(11) NOT NULL,
  INDEX `fk_mod_bam_contact_command_contact1_idx` (`contact_contact_id` ASC),
  INDEX `fk_mod_bam_contact_command_command1_idx` (`command_command_id` ASC),
  PRIMARY KEY (`contact_contact_id`),
  CONSTRAINT `fk_mod_bam_contact_command_contact1`
    FOREIGN KEY (`contact_contact_id`)
      REFERENCES `contact` (`contact_id`)
      ON DELETE CASCADE
      ON UPDATE NO ACTION,
  CONSTRAINT `fk_mod_bam_contact_command_command1`
    FOREIGN KEY (`command_command_id`)
      REFERENCES `command` (`command_id`)
      ON DELETE CASCADE
      ON UPDATE NO ACTION
) ENGINE = InnoDB DEFAULT CHARACTER SET = utf8;

--
-- Create a relation between a contact and a notification period
--
CREATE TABLE IF NOT EXISTS `mod_bam_contact_timeperiod` (
  `contact_contact_id` INT(11) NOT NULL,
  `timeperiod_tp_id` INT(11) NOT NULL,
  PRIMARY KEY (`contact_contact_id`),
  INDEX `fk_mod_bam_contact_timeperiod_contact1_idx` (`contact_contact_id` ASC),
  INDEX `fk_mod_bam_contact_timeperiod_timeperiod1_idx` (`timeperiod_tp_id` ASC),
  CONSTRAINT `fk_mod_bam_contact_timeperiod_contact1`
   FOREIGN KEY (`contact_contact_id`)
     REFERENCES `contact` (`contact_id`)
     ON DELETE CASCADE
     ON UPDATE NO ACTION,
  CONSTRAINT `fk_mod_bam_contact_timeperiod_timeperiod1`
   FOREIGN KEY (`timeperiod_tp_id`)
     REFERENCES `timeperiod` (`tp_id`)
     ON DELETE CASCADE
     ON UPDATE NO ACTION
) ENGINE = InnoDB DEFAULT CHARACTER SET = utf8;

-- 
-- Structure de la table `mod_bam_reporting`
-- 

CREATE TABLE IF NOT EXISTS @DB_CENTSTORAGE@.`mod_bam_reporting` (
  `log_id` int(11) NOT NULL auto_increment,
  `host_name` varchar(255) NOT NULL default '0',
  `service_description` varchar(255) NOT NULL default '0',
  `OKTimeScheduled` int(11) NOT NULL default '0',
  `OKnbEvent` int(11) NOT NULL default '0',
  `OKTimeAverageAck` int(11) NOT NULL,
  `OKTimeAverageRecovery` int(11) NOT NULL,
  `WARNINGTimeScheduled` int(11) NOT NULL default '0',
  `WARNINGnbEvent` int(11) NOT NULL default '0',
  `WARNINGTimeAverageAck` int(11) NOT NULL,
  `WARNINGTimeAverageRecovery` int(11) NOT NULL,
  `UNKNOWNTimeScheduled` int(11) NOT NULL default '0',
  `UNKNOWNnbEvent` int(11) NOT NULL default '0',
  `UNKNOWNTimeAverageAck` int(11) NOT NULL,
  `UNKNOWNTimeAverageRecovery` int(11) NOT NULL,
  `CRITICALTimeScheduled` int(11) NOT NULL default '0',
  `CRITICALnbEvent` int(11) NOT NULL default '0',
  `CRITICALTimeAverageAck` int(11) NOT NULL,
  `CRITICALTimeAverageRecovery` int(11) NOT NULL,
  `UNDETERMINEDTimeScheduled` int(11) NOT NULL default '0',
  `date_start` int(11) default NULL,
  `date_end` int(11) default NULL,
  PRIMARY KEY  (`log_id`),
  KEY `date_end_index` (`date_end`),
  KEY `date_start_index` (`date_start`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- 
-- Structure de la table `mod_bam_reporting_status`
-- 

CREATE TABLE IF NOT EXISTS @DB_CENTSTORAGE@.`mod_bam_reporting_status` (
  `id` int(11) NOT NULL,
  `host_name` varchar(255) default NULL,
  `service_description` varchar(255) default NULL,
  `status` varchar(255) default NULL,
  `ctime` int(11) default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- 
-- Structure de la table `mod_bam_logs`
-- 

CREATE TABLE IF NOT EXISTS @DB_CENTSTORAGE@.`mod_bam_logs` (
  `status` varchar(255) NOT NULL,
  `level` float NOT NULL,
  `warning_thres` float NOT NULL,
  `critical_thres` float NOT NULL,
  `status_change_flag` enum('0','1') NOT NULL default '0',
  `ctime` int(11) NOT NULL,
  `ba_id` int(11) NOT NULL,
  `in_downtime` tinyint(1) NOT NULL default 0,
  `downtime_flag` tinyint(1) NOT NULL default 0,
  KEY `ba_id` (`ba_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- 
-- Structure de la table `mod_bam_kpi_logs`
-- 

CREATE TABLE IF NOT EXISTS @DB_CENTSTORAGE@.`mod_bam_kpi_logs` (
  `kpi_id` int(11),
  `boolean_id` int(11),
  `ba_id` int(11),
  `status` smallint(6) NOT NULL,
  `ctime` int(11) NOT NULL,
  `output` varchar(255) NOT NULL,
  `kpi_name` varchar(255) NOT NULL,
  `kpi_type` enum('0', '1', '2', '3') NOT NULL,
  `impact` float NOT NULL default '0',
  `in_downtime` enum('0', '1') NOT NULL default '0',
  `downtime_flag` tinyint(1) NOT NULL default 0,
  `perfdata` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Business Views.
--
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_bv (
  bv_id int NOT NULL auto_increment,
  bv_name varchar(45) default NULL,

  bv_description text default NULL,

  PRIMARY KEY (bv_id),
  UNIQUE (bv_name)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Business Activities.
--
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_ba (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_kpi (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_relations_ba_bv (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_ba_events (
  ba_event_id int NOT NULL auto_increment,
  ba_id int NOT NULL,
  start_time int NOT NULL,

  first_level double default NULL,
  end_time int default NULL,
  status tinyint default NULL,
  in_downtime boolean default NULL,

  KEY `ba_id_start_time_index` (`ba_id`, `start_time`),
  KEY `ba_id_end_time_index` (`ba_id`, `end_time`),

  PRIMARY KEY (ba_event_id)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- KPI events.
--
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_kpi_events (
  kpi_event_id int NOT NULL auto_increment,
  kpi_id int NOT NULL,
  start_time int NOT NULL,

  end_time int default NULL,
  status tinyint default NULL,
  in_downtime boolean default NULL,
  impact_level tinyint default NULL,
  first_output text default NULL,
  first_perfdata varchar(45) default NULL,

  KEY `kpi_id_start_time_index` (`kpi_id`, `start_time`),

  PRIMARY KEY (kpi_event_id)
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Relations between BA events and KPI events.
--
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_relations_ba_kpi_events (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_timeperiods (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_timeperiods_exceptions (
  timeperiod_id int NOT NULL,
  daterange varchar(255) NOT NULL,
  timerange varchar(255) NOT NULL,

  FOREIGN KEY (timeperiod_id) REFERENCES mod_bam_reporting_timeperiods (timeperiod_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- Timeperiods exclusions.
--
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_timeperiods_exclusions (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_relations_ba_timeperiods (
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
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_ba_events_durations (
  ba_event_id int NOT NULL,
  timeperiod_id int NOT NULL,

  start_time int default NULL,
  end_time int default NULL,
  duration int default NULL,
  sla_duration int default NULL,
  timeperiod_is_default boolean default NULL,

  KEY `end_time_start_time_index` (`end_time`, `start_time`),

  UNIQUE (ba_event_id, timeperiod_id),
  FOREIGN KEY (ba_event_id) REFERENCES mod_bam_reporting_ba_events (ba_event_id)
    ON DELETE CASCADE
) ENGINE=InnoDB CHARACTER SET utf8;

--
-- BA availabilities.
--
CREATE TABLE @DB_CENTSTORAGE@.mod_bam_reporting_ba_availabilities (
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

INSERT INTO `command` (`command_name`,`command_line`,`command_type`,`enable_shell`,`command_example`,`graph_id`) VALUES 
('bam-notify-by-email', '/usr/bin/printf \"%b\" \"***** Centreon BAM *****\\n\\nNotification Type: \$NOTIFICATIONTYPE\$\\n\\nBusiness Activity: \$SERVICEDISPLAYNAME\$\\nState: \$SERVICESTATE\$\\n\\nDate: \$DATE\$ \$TIME\$\\n\\nAdditional Info:\\n\\n\$SERVICEOUTPUT\$\" | @MAILER@ -s \"** \$NOTIFICATIONTYPE\$ - \$SERVICEDISPLAYNAME\$ is \$SERVICESTATE\$ **\" \$CONTACTEMAIL\$',1,1,'',0);

-- 
-- Contenu de la table `mod_bam_user_preferences`
-- 
INSERT INTO `mod_bam_user_preferences` (`user_id`, `pref_key`, `pref_value`) VALUES 
(NULL, 'kpi_warning_drop', '50'),
(NULL, 'kpi_critical_drop', '100'),
(NULL, 'kpi_unknown_drop', '75'),
(NULL, 'kpi_boolean_drop', '100'),
(NULL, 'ba_warning_threshold', '80'),
(NULL, 'ba_critical_threshold', '70'),
(NULL, 'normal_check_interval', '5'),
(NULL, 'id_reporting_period', (SELECT tp_id FROM timeperiod ORDER BY tp_name = '24x7' DESC, tp_id LIMIT 1)),
(NULL, 'id_notif_period', (SELECT tp_id FROM timeperiod ORDER BY tp_name = '24x7' DESC, tp_id LIMIT 1)),
(NULL, 'height_impacts_tree', '400'),
(NULL, 'command_id', (SELECT command_id FROM command ORDER BY command_name = 'bam-notify-by-email' DESC, command_id LIMIT 1));

-- TOPOLOGY
INSERT INTO `topology` (`topology_name`, `topology_parent`, `topology_page`, `topology_order`, `topology_group`, `topology_url`, `topology_url_opt`, `topology_popup`, `topology_modules`, `topology_show`, `is_react`) VALUES
('Business Activity',  '2', '207', '30', '1', './modules/centreon-bam-server/core/dashboard/dashboard.php', NULL, '0', '1', '1', '0'),
('Views',  '207', NULL, NULL, '1', NULL, NULL, '0', '1', '1', '0'),
('Monitoring', '207', '20701', '10', '1', './modules/centreon-bam-server/core/dashboard/dashboard.php', NULL, NULL, '1', '1', '0'),
('Reporting', '207', '20702', '20', '1', './modules/centreon-bam-server/core/reporting/reporting.php', NULL, NULL, '1', '1', '0'),
('Logs', '207', '20703', '30', '1', './modules/centreon-bam-server/core/logs/logs.php', NULL, NULL, '1', '1', '0'),
('Business Activity', '6', '626', '20', '1', '/configuration/bam/bvs', NULL, '0', '1', '1', '1'),
('Management', '626', NULL, NULL, '1', NULL, NULL, '0', '1', '1', '0'),
('Indicators', '626', '62606', '30', '1', './modules/centreon-bam-server/core/configuration/kpi/configuration_kpi.php', NULL, NULL, '1', '1', '0'),
('Boolean Rules', '626', '62611', '40', '1', './modules/centreon-bam-server/core/configuration/boolean/configuration_boolean.php', NULL, NULL, '1', '1', '0'),
('Dependencies', '626', '62612', '40', '1', './modules/centreon-bam-server/core/configuration/dependencies/configuration_dependencies.php', NULL, NULL, '1', '1', '0'),
('Options', '626', NULL, NULL, '2', NULL, NULL, '0', '1', '1', '0'),
('Default Settings', '626', '62607', '10', '2', './modules/centreon-bam-server/core/options/general/general.php', NULL, NULL, '1', '1', '0'),
('User Settings', '626', '62608', '10', '2', './modules/centreon-bam-server/core/options/user/user.php', NULL, NULL, '1', '1', '0'),
('Help', '626', NULL, NULL, '3', NULL, NULL, '0', '1', '1', '0'),
('Troubleshooter', '626', '62610', '10', '3', './modules/centreon-bam-server/core/help/troubleshooter/troubleshooter.php', NULL, NULL, '1', '1', '0');

-- Business Activity Topology
INSERT INTO `topology` (`topology_name`, `topology_parent`, `topology_page`, `topology_order`, `topology_group`, `topology_url`, `topology_url_opt`, `topology_popup`, `topology_modules`, `topology_show`, `is_react`) VALUES
('Business Activity', '626', '62605', '5', '1', '/configuration/bam/bas', NULL, NULL, '1', '1', '1');

INSERT INTO `topology` (`topology_name`, `topology_parent`, `topology_page`, `topology_order`, `topology_group`, `topology_url`, `topology_url_opt`, `topology_popup`, `topology_modules`, `topology_show`, `is_react`) VALUES
('Business Views', '626', '62604', '5', '1', '/configuration/bam/bvs', NULL, NULL, '1', '1', '1');

-- TOPOLOGY_JS
INSERT INTO `topology_JS` (`id_page`, `o`, `PathName_js`, `Init`) VALUES 
('62605', NULL , './include/common/javascript/changetab.js ', 'initChangeTab'),
('20702', NULL , './include/common/javascript/Timeline/src/main/webapp/api/timeline-api.js', 'initTimeline'),
('207','d','./include/common/javascript/changetab.js','initChangeTab'),
('20701','d','./include/common/javascript/changetab.js','initChangeTab');
-- 
-- Contenu de la table `giv_components_template`
-- 
INSERT INTO `giv_components_template` (`name`, `ds_order`, `ds_name`, `ds_color_line`, `ds_color_area`, `ds_filled`, `ds_max`, `ds_min`, `ds_average`, `ds_last`, `ds_tickness`, `ds_transparency`, `ds_invert`, `default_tpl1`, `comment`) VALUES
('BA_Level', NULL, 'BA_Level', '#597f00', '#88b917', '1', '1', '1', '1', '1', 1, '50', NULL, NULL, NULL);

-- 
-- Contenu de la table `giv_graphs_template`
-- 
INSERT INTO `giv_graphs_template` (`name`, `vertical_label`, `width`, `height`, `base`, `lower_limit`, `upper_limit`,  `stacked`, `split_component`, `comment`) VALUES ('Centreon BAM', 'Business Activity', '600', '200', '1000', '0', '110', '0', '0', NULL); 

--
-- Contenu de la table `mod_bam_impacts`
--
INSERT INTO `mod_bam_impacts` (`code`, `impact`, `color`) VALUES
(0, 0, '#ffffff'),
(1, 5, '#ffeebb'),
(2, 25, '#ffcc77'),
(3, 50, '#ff8833'),
(4, 75, '#ff5511'),
(5, 100, '#ff0000');

ALTER TABLE mod_bam_dep_child_relation ADD CONSTRAINT fk_id_ba_child FOREIGN KEY (`id_ba`) REFERENCES mod_bam(`ba_id`) ON DELETE CASCADE ON UPDATE NO ACTION;
ALTER TABLE mod_bam_dep_child_relation ADD CONSTRAINT fk_id_dep_child FOREIGN KEY (`id_dep`) REFERENCES dependency(`dep_id`) ON DELETE CASCADE ON UPDATE NO ACTION;

CREATE VIEW mod_bam_view_kpi AS
SELECT k.kpi_id, b.ba_id, k.activate AS kpi_activate, b.activate AS ba_activate, b.name AS ba_name,
k.host_id, h.host_name AS kpi_host, k.service_id, s.service_description AS kpi_service,
k.id_indicator_ba, bk.name AS kpi_ba,
k.meta_id, meta.meta_name AS kpi_meta,
k.boolean_id, boo.name AS kpi_boolean,
b.icon_id, k.kpi_type, k.config_type,
k.last_impact, k.last_level, k.last_state_change,
k.current_status, k.in_downtime, k.acknowledged,
k.drop_warning, k.drop_critical, k.drop_unknown,
k.drop_warning_impact_id, k.drop_critical_impact_id, k.drop_unknown_impact_id,
b.current_level as ba_current_level, b.current_status as ba_current_status,
b.in_downtime as ba_in_downtime, b.level_w, b.level_c
FROM mod_bam_kpi AS k
INNER JOIN mod_bam AS b ON b.ba_id = k.id_ba
LEFT JOIN host AS h ON h.host_id = k.host_id
LEFT JOIN service AS s ON s.service_id = k.service_id
LEFT JOIN mod_bam AS bk ON bk.ba_id = k.id_indicator_ba
LEFT JOIN meta_service AS meta ON meta.meta_id = k.meta_id
LEFT JOIN mod_bam_boolean as boo ON boo.boolean_id = k.boolean_id
ORDER BY b.name;

