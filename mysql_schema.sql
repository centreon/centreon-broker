-- ----------------------------------------------------------------------------
--                                                                           --
--    After discussing with Andreas Ericsson (who seems to be a nice and     --
--    competent guy), I will try to make CentreonBroker's schema compatible  --
--    with Merlin's (ie. I will copy Merlin's schema into CentreonBroker).   --
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
--   !comment
--   -connection_info
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
--   +host_hostgroup
--   +host_parents
--   +hostdependency
--   +hostescalation
--   +hostescalation_contact
--   +hostescalation_contactgroup
--   +hostgroup
--   -hostgroups
--   -hostgroups_members
--   -hosts_commands
--   -hosts_parenthosts
--   +notification
--   -processevents
--   !program_status
--   +report_data
--   !scheduled_downtime
--   !service
--   +service_contact
--   +service_contactgroup
--   +service_servicegroup
--   +servicedependency
--   +serviceescalation
--   +serviceescalation_contact
--   +serviceescalation_contactgroup
--   +servicegroup
--   -servicegroups
--   -servicegroups_members
--   -services_commands
--   +timeperiod
--   +timeperiod_exclude

--
--
--    Tables compatible with Merlin
--
--

CREATE TABLE IF NOT EXISTS `comment` (
  `id` int(11) NOT NULL auto_increment,               -- OK
  `instance_id` int(11) NOT NULL default '0',         -- OK
  `author_name` varchar(255) default NULL,            -- OK
  `comment_data` text default NULL,                   -- OK
  `comment_type` smallint(6) default NULL,            -- OK (is int in Merlin)
  `entry_time` int(11) default NULL,                  -- OK
  `entry_type` smallint(6) default NULL,              -- OK (is int in Merlin)
  `expire_time` int(11) default NULL,                 -- OK
  `expires` boolean default NULL,                     -- OK (is int in Merlin)
  `host_name` varchar(255) NOT NULL,                  -- OK but why don't we have host_id instead ?
  `persistent` boolean default NULL,                  -- OK
  `service_description` varchar(160) default NULL,    -- OK but why don't we have service_id instead ?
  `source` smallint(6) default NULL,                  -- OK (is int in Merlin)

  `comment_time` int(11) default NULL,                -- not in Merlin
  `deletion_time` int(11) default NULL,               -- not in Merlin
  `internal_comment_id` int(11) default NULL,         -- not in Merlin
  PRIMARY KEY (`id`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `scheduled_downtime` (
  `id` int(11) NOT NULL auto_increment,                -- OK
  `author_name` varchar(255) default NULL,             -- OK
  `comment_data` text default NULL,                    -- OK
  `downtime_id` int(11) default NULL,                  -- OK
  `downtime_type` smallint(6) default NULL,            -- OK (int in Merlin)
  `duration` int(11) default NULL,                     -- OK
  `end_time` int(11) default NULL,                     -- OK
  `entry_time` int(11) default NULL,                   -- OK
  `fixed` boolean default NULL,                        -- OK
  `host_name` varchar(255) NOT NULL,                   -- OK
  `instance_id` int(11) NOT NULL,                      -- OK
  `service_description` varchar(255) NOT NULL,         -- OK
  `start_time` int(11) default NULL,                   -- OK
  `triggered_by` int(11) default NULL,                 -- OK

  `was_cancelled` boolean default NULL,                -- not in Merlin
  `was_started` boolean default NULL,                  -- not in Merlin
  PRIMARY KEY (`id`)
) ENGINE=InnoDB;


--
--
--    Tables with Merlin compatibility under progress
--
--

CREATE TABLE IF NOT EXISTS `host` (
  `id` int(11) NOT NULL auto_increment,                             -- OK
  `instance_id` int(11) NOT NULL ,                                  -- OK
  `host_name` varchar(255) NOT NULL,                                -- OK (varchar(75) in Merlin)
  `acknowledgement_type` smallint(6) default NULL,                  -- OK (int in Merlin)
  `action_url` varchar(255) default NULL,                           -- OK
  `active_checks_enabled` boolean default NULL,                     -- OK
  `address` varchar(75) default NULL,                               -- OK
  `alias` varchar(100) default NULL,                                -- OK
  `check_command` text default NULL,                                -- OK
  `check_freshness` boolean default NULL,                           -- OK
  `check_period` varchar(75) default NULL,                          -- OK
  `check_type` smallint(6) default NULL,                            -- OK (int in Merlin)
  `current_check_attempt` smallint(6) default NULL,                 -- OK (int in Merlin)
  `current_notification_number` smallint(6) default NULL,           -- OK (int in Merlin)
  `current_state` smallint(6) default NULL,                         -- OK (int in Merlin)
  `display_name` varchar(100) default NULL,                         -- OK
  `event_handler_enabled` boolean default NULL,                     -- OK
  `execution_time` double default NULL,                             -- OK (float in Merlin)
  `failure_prediction_enabled` boolean default NULL,                -- OK
  `flap_detection_enabled` boolean default NULL,                    -- OK
  `freshness_threshold` double default NULL,                        -- OK (float in Merlin)
  `has_been_checked` boolean default NULL,                          -- OK (int in Merlin)
  `high_flap_threshold` double default NULL,                        -- OK (float in Merlin)
  `icon_image` varchar(255) default NULL,                           -- OK (varchar(60) in Merlin)
  `icon_image_alt` varchar(255) default NULL,                       -- OK (varchar(60) in Merlin)
  `is_flapping` boolean default NULL,                               -- OK (int in Merlin)
  `last_check` int(11) default NULL,                                -- OK
  `last_hard_state` smallint(6) default NULL,                       -- OK (int in Merlin)
  `last_hard_state_change` int(11) default NULL,                    -- OK
  `last_notification` int(11) default NULL,                         -- OK
  `last_state_change` int(11) default NULL,                         -- OK
  `last_time_down` int(11) default NULL,                            -- OK
  `last_time_unreachable` int(11) default NULL,                     -- OK
  `last_time_up` int(11) default NULL,                              -- OK
  `last_update` int(11) default NULL,                               -- OK
  `latency` double default NULL,                                    -- OK (float in Merlin)
  `low_flap_threshold` double default NULL,                         -- OK (float in Merlin)
  `max_check_attempts` smallint(6) default NULL,                    -- OK
  `modified_attributes` int(11) default NULL,                       -- OK
  `next_check` int(11) default NULL,                                -- OK
  `no_more_notifications` boolean default NULL,                     -- OK (int in Merlin)
  `notes` varchar(255) default NULL,                                -- OK
  `notes_url` varchar(255) default NULL,                            -- OK
  `notifications_enabled` boolean default NULL,                     -- OK
  `obsess_over_host` boolean default NULL,                          -- OK
  `output` text default NULL,                                       -- OK
  `passive_checks_enabled` boolean default NULL,                    -- OK
  `percent_state_change` double default NULL,                       -- OK (float in Merlin)
  `perf_data` text default NULL,                                    -- OK
  `problem_has_been_acknowledged` boolean default NULL,             -- OK (int in Merlin)
  `process_performance_data` boolean default NULL,                  -- OK
  `retain_nonstatus_information` boolean default NULL,              -- OK
  `retain_status_information` boolean default NULL,                 -- OK
  `should_be_scheduled` boolean default NULL,                       -- OK (int in Merlin)
  `state_type` smallint(6) default NULL,                            -- OK (int in Merlin)
  `statusmap_image` varchar(255) default NULL,                      -- OK (varchar(60) in Merlin)
  `vrml_image` varchar(255) default NULL,                           -- OK (varchar(60) in Merlin)

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
  -- last_host_notification int
  -- last_problem_id int
  -- last_state int
  -- long_output text
  -- max_attempts int
  -- next_host_notification int
  -- notification_options varchar(15)
  -- notification_period varchar(75)
  -- pending_flex_downtime int
  -- process_perf_data tinyint(1)
  -- return_code smallint(8)
  -- stalking_options varchar(15)
  -- start_time int
  -- timeout int
  -- total_service_check_interval int
  -- total_services int
  `check_interval` double NOT NULL default '0',                     -- smallint(6)
  `event_handler` varchar(255) NOT NULL default '',                 -- int(11) in Merlin
  `first_notification_delay` double NOT NULL default '0',           -- int
  `flap_detection_on_down` smallint(6) NOT NULL default '0',
  `flap_detection_on_unreachable` smallint(6) NOT NULL default '0',
  `flap_detection_on_up` smallint(6) NOT NULL default '0',
  `have_2d_coords` smallint(6) NOT NULL default '0',
  `next_notification` int(11) NOT NULL,
  `notification_interval` double NOT NULL default '0',              -- mediumint(9)
  `notify_on_down` smallint(6) NOT NULL default '0',                -- notified_on_down int
  `notify_on_downtime` smallint(6) NOT NULL default '0',
  `notify_on_flapping` smallint(6) NOT NULL default '0',
  `notify_on_recovery` smallint(6) NOT NULL default '0',
  `notify_on_unreachable` smallint(6) NOT NULL default '0',         -- notified_on_unreachable int
  `retry_interval` double NOT NULL default '0',                     -- smallint(6)
  `scheduled_downtime_depth` smallint(6) NOT NULL default '0',      -- int
  `stalk_on_down` smallint(6) NOT NULL default '0',
  `stalk_on_unreachable` smallint(6) NOT NULL default '0',
  `stalk_on_up` smallint(6) NOT NULL default '0',
  `x_2d` smallint(6) NOT NULL default '0',
  `y_2d` smallint(6) NOT NULL default '0',

  PRIMARY KEY  (`id`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `program_status` (
  `instance_id` int(11) NOT NULL default '0',                       -- OK
  `instance_name` varchar(255) NOT NULL default 'localhost',        -- OK
  `active_host_checks_enabled` boolean default NULL,                -- OK
  `active_service_checks_enabled` boolean default NULL,             -- OK
  `daemon_mode` boolean default NULL,                               -- OK
  `event_handlers_enabled` boolean default NULL,                    -- OK
  `failure_prediction_enabled` boolean default NULL,                -- OK
  `flap_detection_enabled` boolean default NULL,                    -- OK
  `global_host_event_handler` text default NULL,                    -- OK
  `global_service_event_handler` text default NULL,                 -- OK
  `is_running` boolean default NULL,                                -- OK
  `last_alive` int(11) default NULL,                                -- OK
  `last_command_check` int(11) default NULL,                        -- OK
  `last_log_rotation` int(11) default NULL,                         -- OK
  `modified_host_attributes` int(11) default NULL,                  -- OK
  `modified_service_attributes` int(11) default NULL,               -- OK
  `notifications_enabled` boolean default NULL,                     -- OK
  `obsess_over_hosts` boolean default NULL,                         -- OK
  `obsess_over_services` boolean default NULL,                      -- OK
  `passive_host_checks_enabled` boolean default NULL,               -- OK
  `passive_service_checks_enabled` boolean default NULL,            -- OK
  `pid` int(11) default NULL,                                       -- OK
  `process_performance_data` boolean default NULL,                  -- OK
  `program_start` int(11) default NULL,                             -- OK

  -- check_host_freshness tinyint(2)
  -- check_service_freshness tinyint(2)
  `instance_address` varchar(120) default NULL,
  `instance_description` varchar(128) default NULL,
  `program_end_time` int(11) NOT NULL,

  PRIMARY KEY `instance_id` (`instance_id`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `service` (
  `id` int(11) NOT NULL auto_increment,                                  -- OK
  `instance_id` int(11) NOT NULL,                                        -- OK
  `host_name` varchar(255) NOT NULL,                                     -- OK (varchar(75) in Merlin)
  `service_description` varchar(255) NOT NULL,                           -- OK (varchar(160) in Merlin)

  `acknowledgement_type` smallint(6) default NULL,                       -- OK (int in Merlin)
  `action_url` varchar(255) default NULL,                                -- OK
  `active_checks_enabled` boolean default NULL,                          -- OK
  `check_command` text default NULL,                                     -- OK
  `check_freshness` boolean default NULL,                                -- OK
  `check_period` varchar(75) default NULL,
  `check_type` smallint(6) default NULL,                                 -- OK (int in Merlin)
  `current_attempt` smallint(6) default NULL,                            -- OK (int in Merlin)
  `current_state` smallint(6) default NULL,                              -- OK (int in Merlin)
  `display_name` varchar(160) default NULL,                              -- OK
  `event_handler_enabled` boolean default NULL,                          -- OK
  `execution_time` double default NULL,                                  -- OK (float in Merlin)
  `failure_prediction_enabled` boolean default NULL,                     -- OK
  `flap_detection_enabled` boolean default NULL,                         -- OK
  `has_been_checked` boolean default NULL,                               -- OK (int in Merlin)
  `high_flap_threshold` double default NULL,                             -- OK (float in Merlin)
  `icon_image` varchar(255) default NULL,                                -- OK (varchar(60) in Merlin)
  `icon_image_alt` varchar(255) default NULL,                            -- OK (varchar(60) in Merlin)
  `is_flapping` boolean default NULL,                                    -- OK (int in Merlin)
  `is_volatile` boolean default NULL,                                    -- OK
  `last_check` int(11) default NULL,                                     -- OK
  `last_hard_state_change` int(11) default NULL,                         -- OK
  `last_notification` int(11) default NULL,                              -- OK
  `last_state_change` int(11) default NULL,                              -- OK
  `last_time_critical` int(11) default NULL,                             -- OK
  `last_time_ok` int(11) default NULL,                                   -- OK
  `last_time_unknown` int(11) default NULL,                              -- OK
  `last_time_warning` int(11) default NULL,                              -- OK
  `last_update` int(11) default NULL,                                    -- OK
  `latency` double default NULL,                                         -- OK (float in Merlin)
  `low_flap_threshold` double default NULL,                              -- OK (float in Merlin)
  `max_check_attempts` smallint(6) default NULL,                         -- OK
  `modified_attributes` int(11) default NULL,                            -- OK
  `next_check` int(11) default NULL,                                     -- OK
  `next_notification` int(11) default NULL,                              -- OK
  `no_more_notifications` boolean default NULL,                          -- OK (int in Merlin)
  `notifications_enabled` boolean default NULL,                          -- OK
  `obsess_over_service` boolean default NULL,                            -- OK
  `output` text default NULL,                                            -- OK
  `notes` varchar(255) default NULL,                                     -- OK
  `notes_url` varchar(255) default NULL,                                 -- OK
  `passive_checks_enabled` boolean default NULL,                         -- OK
  `percent_state_change` double default NULL,                            -- OK (float in Merlin)
  `perf_data` text default NULL,                                         -- OK
  `problem_has_been_acknowledged` boolean default NULL,                  -- OK (int in Merlin)
  `process_performance_data` boolean default NULL,                       -- OK
  `retain_nonstatus_information` boolean default NULL,                   -- OK
  `retain_status_information` boolean default NULL,                      -- OK
  `should_be_scheduled` boolean default NULL,                            -- OK (int in Merlin)
  `state_type` smallint(6) default NULL,                                 -- OK (int in Merlin)


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
  -- long_output text
  -- max_attempts int
  -- notification_options
  -- notification_period
  -- notified_on_unknown int
  -- notified_on_warning int
  -- notified_on_critical int
  -- parallelize_check tinyint(1)
  -- pending_flex_downtime int
  -- process_perf_data tinyint(1) XXX : what about process_performance_data ?
  -- return_code smallint
  -- stalking_options
  -- start_time int
  -- timeout int
  `check_interval` double NOT NULL default '0',                          -- smallint(6)
  `current_notification_number` smallint(6) NOT NULL default '0',        -- int
  `default_active_checks_enabled` smallint(6) NOT NULL default '0',
  `default_event_handler_enabled` smallint(6) NOT NULL default '0',
  `default_failure_prediction_enabled` smallint(6) NOT NULL default '0',
  `default_flap_detection_enabled` smallint(6) NOT NULL default '0',
  `default_notifications_enabled` smallint(6) NOT NULL default '0',
  `default_passive_checks_enabled` smallint(6) NOT NULL default '0',
  `default_process_performance_data` smallint(6) NOT NULL default '0',
  `event_handler` varchar(255) NOT NULL default '',                      -- int
  `failure_prediction_options` varchar(64) NOT NULL default '',
  `first_notification_delay` double NOT NULL default '0',                -- int
  `flap_detection_on_critical` smallint(6) NOT NULL default '0',
  `flap_detection_on_ok` smallint(6) NOT NULL default '0',
  `flap_detection_on_unknown` smallint(6) NOT NULL default '0',
  `flap_detection_on_warning` smallint(6) NOT NULL default '0',
  `freshness_threshold` smallint(6) NOT NULL default '0',                -- int, WTF ? double in host table
  `graph_id` int(11) NOT NULL,                                           -- not in Merlin
  `host_id` int(11) NOT NULL,                                            -- not in Merlin, Centreon-specific, fetched from customvars
  `last_hard_state` smallint(6) NOT NULL default '0',                    -- int, why ?
  `notification_interval` double NOT NULL default '0',                   -- int
  `notify_on_critical` smallint(6) NOT NULL default '0',
  `notify_on_downtime` smallint(6) NOT NULL default '0',
  `notify_on_flapping` smallint(6) NOT NULL default '0',
  `notify_on_recovery` smallint(6) NOT NULL default '0',
  `notify_on_unknown` smallint(6) NOT NULL default '0',
  `notify_on_warning` smallint(6) NOT NULL default '0',
  `retry_interval` double NOT NULL default '0',                          -- smallint(6)
  `service_id` int(11) NOT NULL,                                         -- not in Merlin, Centreon-specific, fetched from customvars
  `scheduled_downtime_depth` smallint(6) NOT NULL default '0',           -- int
  `stalk_on_critical` smallint(6) NOT NULL default '0',
  `stalk_on_ok` smallint(6) NOT NULL default '0',
  `stalk_on_unknown` smallint(6) NOT NULL default '0',
  `stalk_on_warning` smallint(6) NOT NULL default '0',

  PRIMARY KEY (`id`)
) ENGINE=InnoDB;


--
--
--    Tables incompatible with Merlin yet
--
--

CREATE TABLE IF NOT EXISTS `acknowledgements` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  `host_id` smallint(5) unsigned default NULL,
  `service_id` smallint(5) unsigned default NULL,
  `entry_time` int(11) NOT NULL,
  `acknowledgement_type` smallint(6) NOT NULL default '0',
  `state` smallint(6) NOT NULL default '0',
  `author_name` varchar(64) NOT NULL default '',
  `comment_data` varchar(255) NOT NULL default '',
  `is_sticky` smallint(6) NOT NULL default '0',
  `persistent_comment` smallint(6) NOT NULL default '0',
  `notify_contacts` smallint(6) NOT NULL default '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Current and historical host and service acknowledgements' ;


CREATE TABLE IF NOT EXISTS `connection_info` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  `agent_name` varchar(32) NOT NULL default '',
  `agent_version` varchar(8) NOT NULL default '',
  -- disposition varchar(16) (not needed ?)
  `connect_source` varchar(16) NOT NULL default '',
  `connect_type` varchar(16) NOT NULL default '',
  `connect_time` int(11) NOT NULL,
  `disconnect_time` int(11) NOT NULL,
  `last_checkin_time` int(11) NOT NULL,
  `data_start_time` int(11) NOT NULL,
  `data_end_time` int(11) NOT NULL,
  `bytes_processed` int(11) NOT NULL default '0',
  `lines_processed` int(11) NOT NULL default '0',
  `entries_processed` int(11) NOT NULL default '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci ;


CREATE TABLE IF NOT EXISTS `customvariables` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  -- XXX : fail ! There shouldn't be any object_id anymore
  `object_id` int(11) NOT NULL default '0',
  `config_type` smallint(6) NOT NULL default '0',
  `has_been_modified` smallint(6) NOT NULL default '0',
  `varname` varchar(255) NOT NULL default '',
  `varvalue` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `object_id_2` (`object_id`,`config_type`,`varname`),
  KEY `varname` (`varname`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Custom variables' ;


CREATE TABLE IF NOT EXISTS `customvariablestatus` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  -- XXX : fail ! There shouldn't be any object_id anymore
  `object_id` int(11) NOT NULL default '0',
  `status_update_time` int(11) NOT NULL,
  `has_been_modified` smallint(6) NOT NULL default '0',
  `varname` varchar(255) NOT NULL default '',
  `varvalue` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `object_id_2` (`object_id`,`varname`),
  KEY `varname` (`varname`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Custom variable status information';


CREATE TABLE IF NOT EXISTS `eventhandlers` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  `host_id` smallint(5) unsigned default NULL,
  `service_id` smallint(5) unsigned default NULL,
  `eventhandler_type` smallint(6) NOT NULL default '0',
  `state` smallint(6) NOT NULL default '0',
  `state_type` smallint(6) NOT NULL default '0',
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  -- XXX : command_object_id is not present but shouldn't it be replaced ?
  `command_args` varchar(255) NOT NULL default '',
  `command_line` varchar(255) NOT NULL default '',
  `timeout` smallint(6) NOT NULL default '0',
  `early_timeout` smallint(6) NOT NULL default '0',
  `execution_time` double NOT NULL default '0',
  `return_code` smallint(6) NOT NULL default '0',
  `output` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `instance_id` (`instance_id`,`start_time`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Historical host and service event handlers' ;


CREATE TABLE IF NOT EXISTS `flappinghistory` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  `host_id` smallint(5) unsigned NOT NULL,
  `service_id` smallint(5) unsigned NOT NULL,
  `event_time` int(11) NOT NULL,
  `event_type` smallint(6) NOT NULL default '0',
  `reason_type` smallint(6) NOT NULL default '0',
  `flapping_type` smallint(6) NOT NULL default '0',
  `percent_state_change` double NOT NULL default '0',
  `low_threshold` double NOT NULL default '0',
  `high_threshold` double NOT NULL default '0',
  `comment_time` int(11) NOT NULL,
  `internal_comment_id` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Current and historical record of host and service flapping' ;


-- XXX : does not match the original definition at all
CREATE TABLE IF NOT EXISTS `hostgroups` (
  `id` int(11) NOT NULL auto_increment,
  `alias` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `enabled` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Hostgroup definitions' ;


-- XXX : does not match the original definition at all
CREATE TABLE IF NOT EXISTS `hostgroups_members` (
  `hostgroup_id` smallint(5) unsigned NOT NULL default '0',
  `host_id` smallint(5) unsigned default NULL,
  `hostgroup_child_id` smallint(6) default NULL,
  PRIMARY KEY  (`hostgroup_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `hosts_commands` (
  `host_id` smallint(5) unsigned NOT NULL default '0',
  `check_command` text,
  `event_handler_command` text,
  PRIMARY KEY  (`host_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `hosts_parenthosts` (
  `host_id` int(11) NOT NULL default '0',
  `host_child_id` int(11) NOT NULL default '0',
  UNIQUE KEY `instance_id` (`host_id`,`host_child_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Parent hosts';


CREATE TABLE IF NOT EXISTS `processevents` (
  `id` int(11) NOT NULL auto_increment,
  `instance_id` int(11) NOT NULL default '0',
  `event_type` smallint(6) NOT NULL default '0',
  `event_time` int(11) NOT NULL,
  `event_time_usec` int(11) NOT NULL default '0',
  `process_id` int(11) NOT NULL default '0',
  `program_name` varchar(16) NOT NULL default '',
  `program_version` varchar(20) NOT NULL default '',
  `program_date` varchar(10) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Historical Nagios process events';


CREATE TABLE IF NOT EXISTS `servicegroups` (
  `id` int(11) NOT NULL auto_increment,
  `alias` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `enabled` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='ServiceGroup definitions' ;


CREATE TABLE IF NOT EXISTS `servicegroups_members` (
  `servicegroup_id` smallint(5) unsigned NOT NULL default '0',
  `service_id` smallint(5) unsigned default NULL,
  `servicegroup_child_id` smallint(5) unsigned default NULL,
  PRIMARY KEY  (`servicegroup_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `services_commands` (
  `service_id` smallint(5) unsigned NOT NULL default '0',
  `check_command` text,
  `event_handler_command` text,
  PRIMARY KEY  (`service_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;
