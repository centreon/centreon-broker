-- ----------------------------------------------------------------------------
--                                                                           --
--    After discussing with Andreas Ericsson (who seems to be a nice and     --
--    competent guy), CentreonBroker will try to adopt Merlin's database     --
--    schema (ie. Merlin's schema will be kind of imported into              --
--    CentreonBroker).                                                       --
--                                                                           --
-- ----------------------------------------------------------------------------

-- Here's the table diff :
--
-- - is a table only in CentreonBroker
-- + is a table only in Merlin
-- ! is a table on both but not compatible
--
--   -acknowledgements
--   +command
--    comment
--   +contact
--   +contact_contactgroup
--   +contactgroup
--   +custom_vars
--   -customvariables
--   -customvariablesstatus
--   +db_version
--   -eventhandlers
--   -flappinghistory
--   +gui_access
--   +gui_action_log
--   !host
--   +host_contact
--   +host_contactgroup
--    host_hostgroup
--    host_parents
--   +hostdependency
--   +hostescalation
--   +hostescalation_contact
--   +hostescalation_contactgroup
--    hostgroup
--   -hosts_commands
--   -log
--   +notification
--   -processevents
--   !program_status
--   +report_data
--    scheduled_downtime
--   !service
--   +service_contact
--   +service_contactgroup
--    service_servicegroup
--   +servicedependency
--   +serviceescalation
--   +serviceescalation_contact
--   +serviceescalation_contactgroup
--    servicegroup
--   -services_commands
--   +timeperiod
--   +timeperiod_exclude

--
--
--    Tables compatible with Merlin
--
--

CREATE TABLE IF NOT EXISTS `comment` (
  `id` int NOT NULL auto_increment,                -- OK
  `instance_id` int NOT NULL default '0',          -- OK
  `author_name` varchar(255) default NULL,         -- OK
  `comment_data` text default NULL,                -- OK
  `comment_type` smallint default NULL,            -- OK (is int in Merlin)
  `entry_time` int default NULL,                   -- OK
  `entry_type` smallint default NULL,              -- OK (is int in Merlin)
  `expire_time` int default NULL,                  -- OK
  `expires` boolean default NULL,                  -- OK (is int in Merlin)
  `host_name` varchar(255) NOT NULL,               -- OK but why don't we have host_id instead ?
  `persistent` boolean default NULL,               -- OK
  `service_description` varchar(160) default NULL, -- OK but why don't we have service_id instead ?
  `source` smallint default NULL,                  -- OK (is int in Merlin)

  `comment_time` int default NULL,                 -- not in Merlin
  `deletion_time` int default NULL,                -- not in Merlin
  `internal_id` int default NULL,                  -- not in Merlin
  PRIMARY KEY (`id`),
  UNIQUE KEY (`internal_id`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `host_hostgroup` (
  `host` int NOT NULL,             -- OK
  `hostgroup` int NOT NULL,        -- OK
  UNIQUE KEY (`host`, `hostgroup`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `host_parents` (
  `host` int NOT NULL,           -- OK
  `parents` int NOT NULL,        -- OK
  UNIQUE KEY (`host`, `parents`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `hostgroup` (
  `id` int NOT NULL auto_increment,             -- OK
  `instance_id` int NOT NULL,                   -- OK
  `action_url` varchar(160) default NULL,       -- OK
  `alias` varchar(255) default NULL,            -- OK (varchar(160) in Merlin)
  `hostgroup_name` varchar(255) default NULL,   -- OK (varchar(160) in Merlin)
  `notes` varchar(160) default NULL,            -- OK
  `notes_url` varchar(160) default NULL,        -- OK
  PRIMARY KEY (`id`),
  UNIQUE KEY (`instance_id`, `hostgroup_name`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `scheduled_downtime` (
  `id` int NOT NULL auto_increment,                -- OK
  `author_name` varchar(255) default NULL,         -- OK
  `comment_data` text default NULL,                -- OK
  `downtime_id` int default NULL,                  -- OK
  `downtime_type` smallint default NULL,           -- OK (int in Merlin)
  `duration` int default NULL,                     -- OK
  `end_time` int default NULL,                     -- OK
  `entry_time` int default NULL,                   -- OK
  `fixed` boolean default NULL,                    -- OK
  `host_name` varchar(255) NOT NULL,               -- OK
  `instance_id` int NOT NULL,                      -- OK
  `service_description` varchar(255) default NULL, -- OK
  `start_time` int default NULL,                   -- OK
  `triggered_by` int default NULL,                 -- OK

  `was_cancelled` boolean default NULL,            -- not in Merlin
  `was_started` boolean default NULL,              -- not in Merlin
  PRIMARY KEY (`id`),
  UNIQUE KEY (`downtime_id`, `instance_id`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `service_servicegroup` (
  `service` int NOT NULL,     -- OK
  `servicegroup` int NOT NULL -- OK
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `servicegroup` (
  `id` int NOT NULL auto_increment,              -- OK
  `instance_id` int NOT NULL,                    -- OK
  `action_url` varchar(160) default NULL,        -- OK
  `alias` varchar(255) default NULL,             -- OK (varchar(160) in Merlin)
  `notes` varchar(160) default NULL,             -- OK
  `notes_url` varchar(160) default NULL,         -- OK
  `servicegroup_name` varchar(255) default NULL, -- OK (varchar(75) in Merlin)
  PRIMARY KEY (`id`)
) ENGINE=InnoDB;


--
--
--    Tables with Merlin compatibility under progress
--
--

CREATE TABLE IF NOT EXISTS `host` (
  `id` int NOT NULL auto_increment,                     -- OK
  `instance_id` int NOT NULL ,                          -- OK
  `host_name` varchar(255) NOT NULL,                    -- OK (varchar(75) in Merlin)
  `acknowledgement_type` smallint default NULL,         -- OK (int in Merlin)
  `action_url` varchar(255) default NULL,               -- OK
  `active_checks_enabled` boolean default NULL,         -- OK
  `address` varchar(75) default NULL,                   -- OK
  `alias` varchar(100) default NULL,                    -- OK
  `check_command` text default NULL,                    -- OK
  `check_freshness` boolean default NULL,               -- OK
  `check_period` varchar(75) default NULL,              -- OK
  `check_type` smallint default NULL,                   -- OK (int in Merlin)
  `current_check_attempt` smallint default NULL,        -- OK (int in Merlin)
  `current_notification_number` smallint default NULL,  -- OK (int in Merlin)
  `current_state` smallint default NULL,                -- OK (int in Merlin)
  `display_name` varchar(100) default NULL,             -- OK
  `event_handler_enabled` boolean default NULL,         -- OK
  `execution_time` double default NULL,                 -- OK (float in Merlin)
  `failure_prediction_enabled` boolean default NULL,    -- OK
  `flap_detection_enabled` boolean default NULL,        -- OK
  `freshness_threshold` double default NULL,            -- OK (float in Merlin)
  `has_been_checked` boolean default NULL,              -- OK (int in Merlin)
  `high_flap_threshold` double default NULL,            -- OK (float in Merlin)
  `icon_image` varchar(255) default NULL,               -- OK (varchar(60) in Merlin)
  `icon_image_alt` varchar(255) default NULL,           -- OK (varchar(60) in Merlin)
  `is_flapping` boolean default NULL,                   -- OK (int in Merlin)
  `last_check` int default NULL,                        -- OK
  `last_hard_state` smallint default NULL,              -- OK (int in Merlin)
  `last_hard_state_change` int default NULL,            -- OK
  `last_notification` int default NULL,                 -- OK
  `last_state_change` int default NULL,                 -- OK
  `last_time_down` int default NULL,                    -- OK
  `last_time_unreachable` int default NULL,             -- OK
  `last_time_up` int default NULL,                      -- OK
  `last_update` int default NULL,                       -- OK
  `latency` double default NULL,                        -- OK (float in Merlin)
  `long_output` text default NULL,                      -- OK
  `low_flap_threshold` double default NULL,             -- OK (float in Merlin)
  `max_check_attempts` smallint default NULL,           -- OK
  `modified_attributes` int default NULL,               -- OK
  `next_check` int default NULL,                        -- OK
  `next_host_notification` int default NULL,            -- OK
  `no_more_notifications` boolean default NULL,         -- OK (int in Merlin)
  `notes` varchar(255) default NULL,                    -- OK
  `notes_url` varchar(255) default NULL,                -- OK
  `notification_period` varchar(75) default NULL,       -- OK
  `notifications_enabled` boolean default NULL,         -- OK
  `obsess_over_host` boolean default NULL,              -- OK
  `output` text default NULL,                           -- OK
  `passive_checks_enabled` boolean default NULL,        -- OK
  `percent_state_change` double default NULL,           -- OK (float in Merlin)
  `perf_data` text default NULL,                        -- OK
  `problem_has_been_acknowledged` boolean default NULL, -- OK (int in Merlin)
  `process_performance_data` boolean default NULL,      -- OK
  `retain_nonstatus_information` boolean default NULL,  -- OK
  `retain_status_information` boolean default NULL,     -- OK
  `should_be_scheduled` boolean default NULL,           -- OK (int in Merlin)
  `state_type` smallint default NULL,                   -- OK (int in Merlin)
  `statusmap_image` varchar(255) default NULL,          -- OK (varchar(60) in Merlin)
  `vrml_image` varchar(255) default NULL,               -- OK (varchar(60) in Merlin)

  -- 2d_coords varchar(20)
  -- 3d_coords varchar(20)
  -- check_flapping_recovery_notification int
  -- check_options int
  -- current_event_id int
  -- current_notification_id int
  -- current_problem_id int
  -- early_timeout smallint(1)
  -- end_time int
  -- event_handler_args text in Merlin
  -- flap_detection_options varchar(18)
  -- flapping_comment_id int
  -- initial_state varchar(18) in Merlin
  -- is_being_freshened int
  -- is_executing int
  -- last_event_id
  -- last_problem_id int
  -- last_state int
  -- max_attempts int
  -- notification_options varchar(15)
  -- pending_flex_downtime int
  -- return_code smallint(8)
  -- stalking_options varchar(15)
  -- start_time int
  -- timeout int
  -- total_service_check_interval int
  -- total_services int
  `check_interval` double NOT NULL default '0',         -- smallint
  `event_handler` varchar(255) NOT NULL default '',     -- int in Merlin
  `first_notification_delay` double default NULL,       -- int
  `flap_detection_on_down` smallint default NULL,
  `flap_detection_on_unreachable` smallint default NULL,
  `flap_detection_on_up` smallint NOT NULL default '0',
  `have_2d_coords` smallint NOT NULL default '0',
  `host_id` int NOT NULL default '0',                   -- not in Merlin, fetched from custom vars
  `notification_interval` double NOT NULL default '0',  -- mediumint(9)
  `notify_on_down` smallint NOT NULL default '0',       -- notified_on_down int
  `notify_on_downtime` smallint NOT NULL default '0',
  `notify_on_flapping` smallint NOT NULL default '0',
  `notify_on_recovery` smallint NOT NULL default '0',
  `notify_on_unreachable` smallint default NULL,        -- notified_on_unreachable int
  `retry_interval` double NOT NULL default '0',         -- smallint
  `scheduled_downtime_depth` smallint default NULL,     -- int
  `stalk_on_down` smallint NOT NULL default '0',
  `stalk_on_unreachable` smallint NOT NULL default '0',
  `stalk_on_up` smallint NOT NULL default '0',
  `x_2d` smallint NOT NULL default '0',
  `y_2d` smallint NOT NULL default '0',

  PRIMARY KEY (`id`),
  INDEX (`instance_id`, `host_name`),
  UNIQUE (`instance_id`, `host_name`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `program_status` (
  `instance_id` int NOT NULL auto_increment,                 -- OK
  `instance_name` varchar(255) NOT NULL default 'localhost', -- OK
  `active_host_checks_enabled` boolean default NULL,         -- OK
  `active_service_checks_enabled` boolean default NULL,      -- OK
  `daemon_mode` boolean default NULL,                        -- OK
  `event_handlers_enabled` boolean default NULL,             -- OK
  `failure_prediction_enabled` boolean default NULL,         -- OK
  `flap_detection_enabled` boolean default NULL,             -- OK
  `global_host_event_handler` text default NULL,             -- OK
  `global_service_event_handler` text default NULL,          -- OK
  `is_running` boolean default NULL,                         -- OK
  `last_alive` int default NULL,                             -- OK
  `last_command_check` int default NULL,                     -- OK
  `last_log_rotation` int default NULL,                      -- OK
  `modified_host_attributes` int default NULL,               -- OK
  `modified_service_attributes` int default NULL,            -- OK
  `notifications_enabled` boolean default NULL,              -- OK
  `obsess_over_hosts` boolean default NULL,                  -- OK
  `obsess_over_services` boolean default NULL,               -- OK
  `passive_host_checks_enabled` boolean default NULL,        -- OK
  `passive_service_checks_enabled` boolean default NULL,     -- OK
  `pid` int default NULL,                                    -- OK
  `process_performance_data` boolean default NULL,           -- OK
  `program_start` int default NULL,                          -- OK

  -- check_host_freshness tinyint(2)
  -- check_service_freshness tinyint(2)
  `instance_address` varchar(120) default NULL,              -- not in Merlin
  `instance_description` varchar(128) default NULL,          -- not in Merlin
  `program_end` int default NULL,                            -- not in Merlin

  PRIMARY KEY (`instance_id`),
  UNIQUE KEY (`instance_name`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `service` (
  `id` int NOT NULL auto_increment,                     -- OK
  `instance_id` int NOT NULL,                           -- OK
  `host_name` varchar(255) NOT NULL,                    -- OK (varchar(75) in Merlin)
  `service_description` varchar(255) NOT NULL,          -- OK (varchar(160) in Merlin)

  `acknowledgement_type` smallint default NULL,         -- OK (int in Merlin)
  `action_url` varchar(255) default NULL,               -- OK
  `active_checks_enabled` boolean default NULL,         -- OK
  `check_command` text default NULL,                    -- OK
  `check_freshness` boolean default NULL,               -- OK
  `check_period` varchar(75) default NULL,              -- OK
  `check_type` smallint default NULL,                   -- OK (int in Merlin)
  `current_attempt` smallint default NULL,              -- OK (int in Merlin)
  `current_notification_number` smallint default NULL,  -- OK (int in Merlin)
  `current_state` smallint default NULL,                -- OK (int in Merlin)
  `display_name` varchar(160) default NULL,             -- OK
  `event_handler_enabled` boolean default NULL,         -- OK
  `execution_time` double default NULL,                 -- OK (float in Merlin)
  `failure_prediction_enabled` boolean default NULL,    -- OK
  `flap_detection_enabled` boolean default NULL,        -- OK
  `has_been_checked` boolean default NULL,              -- OK (int in Merlin)
  `high_flap_threshold` double default NULL,            -- OK (float in Merlin)
  `icon_image` varchar(255) default NULL,               -- OK (varchar(60) in Merlin)
  `icon_image_alt` varchar(255) default NULL,           -- OK (varchar(60) in Merlin)
  `is_flapping` boolean default NULL,                   -- OK (int in Merlin)
  `is_volatile` boolean default NULL,                   -- OK
  `last_check` int default NULL,                        -- OK
  `last_hard_state` smallint default NULL,              -- OK (int in Merlin)
  `last_hard_state_change` int default NULL,            -- OK
  `last_notification` int default NULL,                 -- OK
  `last_state_change` int default NULL,                 -- OK
  `last_time_critical` int default NULL,                -- OK
  `last_time_ok` int default NULL,                      -- OK
  `last_time_unknown` int default NULL,                 -- OK
  `last_time_warning` int default NULL,                 -- OK
  `last_update` int default NULL,                       -- OK
  `latency` double default NULL,                        -- OK (float in Merlin)
  `long_output` text default NULL,                      -- OK
  `low_flap_threshold` double default NULL,             -- OK (float in Merlin)
  `max_check_attempts` smallint default NULL,           -- OK
  `modified_attributes` int default NULL,               -- OK
  `next_check` int default NULL,                        -- OK
  `next_notification` int default NULL,                 -- OK
  `no_more_notifications` boolean default NULL,         -- OK (int in Merlin)
  `notification_period` varchar(75) default NULL,       -- OK
  `notifications_enabled` boolean default NULL,         -- OK
  `notified_on_critical` boolean default NULL,          -- OK (int in Merlin)
  `notified_on_unknown` boolean default NULL,           -- OK (int in Merlin)
  `notified_on_warning` boolean default NULL,           -- OK (int in Merlin)
  `obsess_over_service` boolean default NULL,           -- OK
  `output` text default NULL,                           -- OK
  `notes` varchar(255) default NULL,                    -- OK
  `notes_url` varchar(255) default NULL,                -- OK
  `passive_checks_enabled` boolean default NULL,        -- OK
  `percent_state_change` double default NULL,           -- OK (float in Merlin)
  `perf_data` text default NULL,                        -- OK
  `problem_has_been_acknowledged` boolean default NULL, -- OK (int in Merlin)
  `process_performance_data` boolean default NULL,      -- OK
  `retain_nonstatus_information` boolean default NULL,  -- OK
  `retain_status_information` boolean default NULL,     -- OK
  `should_be_scheduled` boolean default NULL,           -- OK (int in Merlin)
  `state_type` smallint default NULL,                   -- OK (int in Merlin)


  -- check_options int
  -- check_flapping_recovery_notification int
  -- current_event_id int
  -- current_notification_id int
  -- current_problem_id
  -- early_timeout smallint
  -- end_time int
  -- event_handler_args text
  -- flap_detection_options varchar(18)
  -- flapping_comment_id
  -- host_problem_at_last_check
  -- initial_state varchar(1)
  -- is_being_freshened int
  -- is_executing int
  -- last_event_id int
  -- last_problem_id
  -- last_state int
  -- max_attempts int
  -- notification_options
  -- parallelize_check tinyint(1)
  -- pending_flex_downtime int
  -- return_code smallint
  -- stalking_options
  -- start_time int
  -- timeout int
  `check_interval` double NOT NULL default '0',                      -- smallint
  `default_active_checks_enabled` smallint NOT NULL default '0',
  `default_event_handler_enabled` smallint NOT NULL default '0',
  `default_failure_prediction_enabled` smallint NOT NULL default '0',
  `default_flap_detection_enabled` smallint NOT NULL default '0',
  `default_notifications_enabled` smallint NOT NULL default '0',
  `default_passive_checks_enabled` smallint NOT NULL default '0',
  `default_process_performance_data` smallint NOT NULL default '0',
  `event_handler` varchar(255) NOT NULL default '',                  -- int
  `failure_prediction_options` varchar(64) NOT NULL default '',
  `first_notification_delay` double NOT NULL default '0',            -- int
  `flap_detection_on_critical` smallint NOT NULL default '0',
  `flap_detection_on_ok` smallint NOT NULL default '0',
  `flap_detection_on_unknown` smallint NOT NULL default '0',
  `flap_detection_on_warning` smallint NOT NULL default '0',
  `freshness_threshold` double default NULL,                         -- int, WTF ? double in host table, smallint originally
  `graph_id` int default NULL,                                       -- not in Merlin
  `host_id` int default NULL,                                        -- not in Merlin, Centreon-specific, fetched from customvars
  `notification_interval` double NOT NULL default '0',               -- int
  `notify_on_downtime` smallint NOT NULL default '0',
  `notify_on_flapping` smallint NOT NULL default '0',
  `notify_on_recovery` smallint NOT NULL default '0',
  `retry_interval` double NOT NULL default '0',                      -- smallint
  `service_id` int default NULL,                                     -- not in Merlin, Centreon-specific, fetched from customvars
  `scheduled_downtime_depth` smallint NOT NULL default '0',          -- int
  `stalk_on_critical` smallint NOT NULL default '0',
  `stalk_on_ok` smallint NOT NULL default '0',
  `stalk_on_unknown` smallint NOT NULL default '0',
  `stalk_on_warning` smallint NOT NULL default '0',

  PRIMARY KEY (`id`),
  INDEX (`instance_id`, `host_name`, `service_description`),
  UNIQUE (`instance_id`, `host_name`, `service_description`)
) ENGINE=InnoDB;


--
--
--    Tables incompatible with Merlin yet
--
--

CREATE TABLE IF NOT EXISTS `acknowledgements` (
  `id` int NOT NULL auto_increment,
  `instance_id` int NOT NULL,
  `host_id` int default NULL,
  `service_id` int default NULL,

  `acknowledgement_type` smallint default NULL,
  `author_name` varchar(64) default NULL,
  `comment_data` varchar(255) default NULL,
  `entry_time` int default NULL,
  `is_sticky` boolean default NULL,
  `notify_contacts` boolean default NULL,
  `persistent_comment` boolean default NULL,
  `state` smallint default NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Current and historical host and service acknowledgements' ;


CREATE TABLE IF NOT EXISTS `customvariables` (
  `id` int NOT NULL auto_increment,
  `instance_id` int NOT NULL default '0',
  -- XXX : fail ! There shouldn't be any object_id anymore
  `object_id` int NOT NULL default '0',
  `config_type` smallint NOT NULL default '0',
  `has_been_modified` smallint NOT NULL default '0',
  `varname` varchar(255) NOT NULL default '',
  `varvalue` varchar(255) NOT NULL default '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `object_id_2` (`object_id`,`config_type`,`varname`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Custom variables' ;


CREATE TABLE IF NOT EXISTS `customvariablestatus` (
  `id` int NOT NULL auto_increment,
  `instance_id` int NOT NULL default '0',
  -- XXX : fail ! There shouldn't be any object_id anymore
  `object_id` int NOT NULL default '0',
  `status_update_time` int NOT NULL,
  `has_been_modified` smallint NOT NULL default '0',
  `varname` varchar(255) NOT NULL default '',
  `varvalue` varchar(255) NOT NULL default '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `object_id_2` (`object_id`,`varname`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Custom variable status information';


CREATE TABLE IF NOT EXISTS `eventhandlers` (
  `id` int NOT NULL auto_increment,
  `instance_id` int NOT NULL default '0',
  `host_id` smallint unsigned default NULL,
  `service_id` smallint unsigned default NULL,
  `eventhandler_type` smallint NOT NULL default '0',
  `state` smallint NOT NULL default '0',
  `state_type` smallint NOT NULL default '0',
  `start_time` int NOT NULL,
  `end_time` int NOT NULL,
  -- XXX : command_object_id is not present but shouldn't it be replaced ?
  `command_args` varchar(255) NOT NULL default '',
  `command_line` varchar(255) NOT NULL default '',
  `timeout` smallint NOT NULL default '0',
  `early_timeout` smallint NOT NULL default '0',
  `execution_time` double NOT NULL default '0',
  `return_code` smallint NOT NULL default '0',
  `output` varchar(255) NOT NULL default '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `instance_id` (`instance_id`,`start_time`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Historical host and service event handlers' ;


CREATE TABLE IF NOT EXISTS `flappinghistory` (
  `id` int NOT NULL auto_increment,
  `instance_id` int NOT NULL default '0',
  `host_id` smallint unsigned NOT NULL,
  `service_id` smallint unsigned NOT NULL,
  `event_time` int NOT NULL,
  `event_type` smallint NOT NULL default '0',
  `reason_type` smallint NOT NULL default '0',
  `flapping_type` smallint NOT NULL default '0',
  `percent_state_change` double NOT NULL default '0',
  `low_threshold` double NOT NULL default '0',
  `high_threshold` double NOT NULL default '0',
  `comment_time` int NOT NULL,
  `internal_comment_id` int NOT NULL default '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Current and historical record of host and service flapping' ;


CREATE TABLE IF NOT EXISTS `hosts_commands` (
  `host_id` smallint unsigned NOT NULL default '0',
  `check_command` text,
  `event_handler_command` text,
  PRIMARY KEY (`host_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `log` (
  `log_id` int NOT NULL auto_increment,
  `ctime` int default NULL,
  `host_name` varchar(64) default NULL,
  `service_description` varchar(64) default NULL,
  `instance` int NOT NULL default '1',
  `msg_type` enum('0','1','2','3','4','5','6','7','8','9','10','11') NOT NULL,
  `notification_cmd` varchar(255) default NULL,
  `notification_contact` varchar(255) default NULL,
  `output` text,
  `retry` int NOT NULL,
  `status` enum('0', '1', '2', '3', '4') default NULL,
  `type` smallint default NULL,

  PRIMARY KEY  (`log_id`),
  KEY `host_name` (`host_name`(64)),
  KEY `service_description` (`service_description`(64)),
  KEY `status` (`status`),
  KEY `instance` (`instance`),
  KEY `ctime` (`ctime`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ;


CREATE TABLE IF NOT EXISTS `processevents` (
  `id` int NOT NULL auto_increment,
  `instance_id` int NOT NULL default '0',
  `event_type` smallint NOT NULL default '0',
  `event_time` int NOT NULL,
  `event_time_usec` int NOT NULL default '0',
  `process_id` int NOT NULL default '0',
  `program_name` varchar(16) NOT NULL default '',
  `program_version` varchar(20) NOT NULL default '',
  `program_date` varchar(10) NOT NULL default '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Historical Nagios process events';


CREATE TABLE IF NOT EXISTS `services_commands` (
  `service_id` smallint unsigned NOT NULL default '0',
  `check_command` text,
  `event_handler_command` text,
  PRIMARY KEY (`service_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;
