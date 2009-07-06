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
--   -instances
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
--    Tables compatible with Merlin (none yet :-( )
--
--

--
--
--    Tables with Merlin compatibility under progress
--
--

CREATE TABLE IF NOT EXISTS `comment` (
  `id` int(11) NOT NULL auto_increment,               -- OK
  `instance_id` int(11) NOT NULL default '0',         -- OK
  `author_name` varchar(255) NOT NULL default '',     -- OK
  `comment_data` text NOT NULL default '',            -- OK
  `comment_time` int(11) NOT NULL,                    -- not in Merlin
  `comment_type` smallint(6) NOT NULL default '0',    -- OK (is int in Merlin)
  `deletion_time` int(11) NOT NULL,                   -- not in Merlin
  `entry_time` int(11) NOT NULL,                      -- OK
  `entry_type` smallint(6) NOT NULL default '0',      -- OK (is int in Merlin)
  `expire_time` int(11) NOT NULL,                     -- OK
  `expires` boolean NOT NULL default '0',             -- OK (is int in Merlin)
  `host_name` varchar(255) default NULL,              -- OK but why don't we have host_id instead ?
  `internal_comment_id` int(11) NOT NULL default '0', -- not in Merlin, where do we fetch it ?
  `persistent` boolean NOT NULL default '0',          -- OK
  `service_description` varchar(160) default NULL,    -- OK but why don't we have service_id instead ?
  `source` smallint(6) NOT NULL default '0',          -- OK (is int in Merlin)
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `host` (
  `id` int(11) NOT NULL auto_increment,                             -- OK
  `instance_id` int(11) NOT NULL default '0',                       -- OK
  `host_name` varchar(255) default NULL,                            -- OK (varchar(75) in Merlin)
  `alias` varchar(100) NOT NULL default '',                         -- OK
  `display_name` varchar(100) NOT NULL default '',                  -- OK
  `address` varchar(75) NOT NULL default '',                        -- OK
  -- initial_state varchar(18) in Merlin
  `check_interval` double NOT NULL default '0',                     -- smallint(6)
  `retry_interval` double NOT NULL default '0',                     -- smallint(6)
  `max_check_attempts` smallint(6) NOT NULL default '0',            -- OK
  -- max_attempts int
  -- current_event_id int
  -- start_time int
  -- end_time int
  -- early_timeout smallint(1)
  -- return_code smallint(8)
  -- last_event_id
  -- check_period varchar(75) in Merlin
  `first_notification_delay` double NOT NULL default '0',           -- int
  -- notification_period varchar(75)
  -- notification_options varchar(15)
  `notification_interval` double NOT NULL default '0',              -- mediumint(9)
  `status_update_time` int(11) NOT NULL,
  -- timeout int
  `output` text NOT NULL default '',                                -- OK
  -- long_output text
  `perf_data` text NOT NULL default '',                             -- OK
  `current_state` smallint(6) NOT NULL default '0',                 -- int, why ?
  `has_been_checked` smallint(6) NOT NULL default '0',              -- int, why ?
  `should_be_scheduled` smallint(6) NOT NULL default '0',           -- int, why ?
  `current_check_attempt` smallint(6) NOT NULL default '0',         -- int
  `last_check` int(11) NOT NULL,                                    -- OK
  `next_check` int(11) NOT NULL,                                    -- OK
  `check_type` smallint(6) NOT NULL default '0',                    -- int
  -- last_state int
  -- last_update int
  `last_state_change` int(11) NOT NULL,                             -- OK
  `last_hard_state_change` int(11) NOT NULL,                        -- OK
  `last_hard_state` smallint(6) NOT NULL default '0',               -- int
  `last_time_up` int(11) NOT NULL,                                  -- OK
  `last_time_down` int(11) NOT NULL,                                -- OK
  `last_time_unreachable` int(11) NOT NULL,                         -- OK
  `state_type` smallint(6) NOT NULL default '0',                    -- int
  `last_notification` int(11) NOT NULL,                             -- OK
  `next_notification` int(11) NOT NULL,
  -- last_host_notification int
  -- next_host_notification int
  `no_more_notifications` smallint(6) NOT NULL default '0',         -- int, why ?
  -- current_notification_id int
  `notifications_enabled` smallint(6) NOT NULL default '0',         -- tinyint(1)
  `problem_has_been_acknowledged` smallint(6) NOT NULL default '0', -- int, why ?
  `acknowledgement_type` smallint(6) NOT NULL default '0',          -- int, why ?
  `current_notification_number` smallint(6) NOT NULL default '0',   -- int
  `passive_checks_enabled` smallint(6) NOT NULL default '0',        -- tinyint(1) in Merlin
  `active_checks_enabled` smallint(6) NOT NULL default '0',         -- tinyint(1) in Merlin
  `event_handler_enabled` smallint(6) NOT NULL default '0',         -- tinyint(1) in Merlin
  `flap_detection_enabled` smallint(6) NOT NULL default '0',        -- tinyint(1) in Merlin
  -- flap_detection_options varchar(18)
  `is_flapping` smallint(6) NOT NULL default '0',                   -- int, why ?
  -- flapping_comment_id int
  `percent_state_change` double NOT NULL default '0',               -- float
  -- total_services int
  -- total_service_check_interval int
  `latency` double NOT NULL default '0',                            -- float
  `execution_time` double NOT NULL default '0',                     -- float
  -- is_executing int
  -- check_options int
  `scheduled_downtime_depth` smallint(6) NOT NULL default '0',      -- int
  -- pending_flex_downtime int
  `failure_prediction_enabled` smallint(6) NOT NULL default '0',    -- tinyint(1)
  `process_performance_data` smallint(6) NOT NULL default '0',      -- int
  -- process_perf_data tinyint(1)
  `obsess_over_host` smallint(6) NOT NULL default '0',              -- tinyint(1) in Merlin
  `modified_attributes` int(11) NOT NULL default '0',               -- OK
  -- current_problem_id int
  -- last_problem_id int
  `event_handler` varchar(255) NOT NULL default '',                 -- int(11) in Merlin
  -- event_handler_args text in Merlin
  `check_command` text NOT NULL default '',                         -- OK
  `notes` varchar(255) default NULL,                                -- OK
  `notes_url` varchar(255) default NULL,                            -- OK
  `action_url` varchar(255) default NULL,                           -- OK
  `icon_image` varchar(255) default NULL,                           -- OK (varchar(60) in Merlin)
  `icon_image_alt` varchar(255) default NULL,                       -- OK (varchar(60) in Merlin)
  `vrml_image` varchar(255) default NULL,                           -- OK (varchar(60) in Merlin)
  `statusmap_image` varchar(255) default NULL,                      -- OK (varchar(60) in Merlin)
  -- 2d_coords varchar(20)
  -- 3d_coords varchar(20)
  `have_2d_coords` smallint(6) NOT NULL default '0',
  `x_2d` smallint(6) NOT NULL default '0',
  `y_2d` smallint(6) NOT NULL default '0',
  `notify_on_down` smallint(6) NOT NULL default '0',                -- notified_on_down int
  `notify_on_unreachable` smallint(6) NOT NULL default '0',         -- notified_on_unreachable int
  `notify_on_recovery` smallint(6) NOT NULL default '0',
  `notify_on_flapping` smallint(6) NOT NULL default '0',
  `notify_on_downtime` smallint(6) NOT NULL default '0',
  `stalk_on_up` smallint(6) NOT NULL default '0',
  `stalk_on_down` smallint(6) NOT NULL default '0',
  `stalk_on_unreachable` smallint(6) NOT NULL default '0',
  -- stalking_options varchar(15)
  `flap_detection_on_up` smallint(6) NOT NULL default '0',
  `flap_detection_on_down` smallint(6) NOT NULL default '0',
  `flap_detection_on_unreachable` smallint(6) NOT NULL default '0',
  `low_flap_threshold` double NOT NULL default '0',                 -- float in Merlin
  `high_flap_threshold` double NOT NULL default '0',                -- float in Merlin
  -- check_freshness tinyint(1)
  -- is_being_freshened int
  -- check_flapping_recovery_notification int
  `freshness_threshold` smallint(6) NOT NULL default '0',           -- float in Merlin
  `retain_status_information` smallint(6) NOT NULL default '0',     -- tinyint(1) in Merlin
  `retain_nonstatus_information` smallint(6) NOT NULL default '0',  -- tinyint(1) in Merlin
  PRIMARY KEY  (`id`),
  UNIQUE KEY `key1` (`instance_id`, `host_name`)                    -- UNIQUE only on host_name
  -- KEY(next_check, host_name)
  -- KEY(last_check, host_name)
  -- KEY(last_state_change, host_name)
  -- KEY(last_hard_state_change, host_name)
  -- KEY(last_time_up, host_name)
  -- KEY(last_time_down, host_name)
  -- KEY(last_time_unreachable, host_name)
  -- KEY(latency, host_name)
  -- KEY(exection_time, host_name)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Host information';


CREATE TABLE IF NOT EXISTS `program_status` (
  `instance_id` int(11) NOT NULL default '0',                       -- OK
  -- instance_name varchar(255)
  -- last_alive int
  `status_update_time` int(11) NOT NULL,
  `program_start_time` int(11) NOT NULL,                            -- program_start int
  `program_end_time` int(11) NOT NULL,
  `is_currently_running` smallint(6) NOT NULL default '0',          -- is_running tinyint(2)
  `pid` int(11) NOT NULL default '0',                               -- OK
  `daemon_mode` smallint(6) NOT NULL default '0',                   -- tinyint(2)
  `last_command_check` int(11) NOT NULL,                            -- OK
  `last_log_rotation` int(11) NOT NULL,                             -- OK
  `notifications_enabled` smallint(6) NOT NULL default '0',         -- tinyint(2)
  `active_service_checks_enabled` smallint(6) NOT NULL default '0', -- tinyint(2)
  `passive_service_checks_enabled` smallint(6) NOT NULL default '0',-- tinyint(2)
  `active_host_checks_enabled` smallint(6) NOT NULL default '0',    -- tinyint(2)
  `passive_host_checks_enabled` smallint(6) NOT NULL default '0',   -- tinyint(2)
  `event_handlers_enabled` smallint(6) NOT NULL default '0',        -- tinyint(2)
  `flap_detection_enabled` smallint(6) NOT NULL default '0',        -- tinyint(2)
  `failure_prediction_enabled` smallint(6) NOT NULL default '0',    -- tinyint(2)
  `process_performance_data` smallint(6) NOT NULL default '0',      -- tinyint(2)
  `obsess_over_hosts` smallint(6) NOT NULL default '0',             -- tinyint(2)
  `obsess_over_services` smallint(6) NOT NULL default '0',          -- tinyint(2)
  -- check_host_freshness tinyint(2)
  -- check_service_freshness tinyint(2)
  `modified_host_attributes` int(11) NOT NULL default '0',          -- OK
  `modified_service_attributes` int(11) NOT NULL default '0',       -- OK
  `global_host_event_handler` text NOT NULL default '',             -- OK
  `global_service_event_handler` text NOT NULL default '',          -- OK
  PRIMARY KEY `instance_id` (`instance_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `scheduled_downtime` (
  `id` int(11) NOT NULL auto_increment,                -- OK
  `instance_id` int(11) NOT NULL default '0',          -- OK
  `downtime_type` smallint(6) NOT NULL default '0',    -- int, why ?
  `host_id` smallint(5) unsigned default NULL,         -- not in Merlin but host_name is
  `service_id` smallint(5) unsigned default NULL,      -- not in Merlin but service_description is
  `entry_time` int(11) NOT NULL,                       -- OK
  `author_name` varchar(255) NOT NULL default '',      -- OK
  `comment_data` text NOT NULL default '',             -- OK
  `internal_downtime_id` int(11) NOT NULL default '0', -- not in Merlin
  `triggered_by` int(11) NOT NULL default '0',         -- OK
  `is_fixed` smallint(6) NOT NULL default '0',         -- fixed tinyint(2)
  `duration` smallint(6) NOT NULL default '0',         -- int
  `start_time` int(11) NOT NULL,                       -- OK
  `end_time` int(11) NOT NULL,                         -- OK
  `was_started` smallint(6) NOT NULL default '0',      -- not in Merlin
  `actual_start_time` int(11) NOT NULL,                -- not in Merlin : what's the purpose of this field against start_time ?
  `actual_end_time` int(11) NOT NULL,                  -- not in Merlin : what's the purpose of this field against end_time ?
  `was_cancelled` smallint(6) NOT NULL default '0',    -- not in Merlin
  -- downtime_id int
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci ;


CREATE TABLE IF NOT EXISTS `service` (
  `id` int(11) NOT NULL auto_increment,                                  -- OK
  `instance_id` int(11) NOT NULL default '0',                            -- OK
  `host_id` int(11) NOT NULL,                                            -- not in Merlin, Centreon-specific, fetched from customvars
  `service_id` int(11) NOT NULL,                                         -- not in Merlin, Centreon-specific, fetched from customvars
  `host_name` varchar(64) NOT NULL default '',                           -- varchar(75)
  `service_description` varchar(160) default NULL,                       -- OK
  `display_name` varchar(160) default NULL,                              -- OK
  -- initial_state varchar(1)
  `graph_id` int(11) NOT NULL,
  `status_update_time` int(11) NOT NULL,
  `check_interval` double NOT NULL default '0',                          -- smallint(6)
  `retry_interval` double NOT NULL default '0',                          -- smallint(6)
  `max_check_attempts` smallint(6) NOT NULL default '0',                 -- smallint(6)
  -- max_attempts int
  -- check_period varchar(75)
  -- parallelize_check tinyint(1)
  `current_check_attempt` smallint(6) NOT NULL default '0',              -- current_attempt int
  -- current_event_id int
  -- last_event_id int
  -- current_problem_id
  -- last_problem_id
  -- is_being_freshened int
  -- is_executing int
  `has_been_checked` smallint(6) NOT NULL default '0',                   -- int, why ?
  `should_be_scheduled` smallint(6) NOT NULL default '0',                -- int, why ?
  `passive_checks_enabled` smallint(6) NOT NULL default '0',             -- tinyint(1)
  `active_checks_enabled` smallint(6) NOT NULL default '0',              -- tinyint(1)
  `current_state` smallint(6) NOT NULL default '0',                      -- int, why ?
  `state_type` smallint(6) NOT NULL default '0',                         -- int, why ?
  `output` varchar(255) NOT NULL default '',                             -- text
  -- long_output                                                         -- text
  `perf_data` text NOT NULL default '',                                  -- OK
  `last_check` int(11) NOT NULL,                                         -- OK
  `next_check` int(11) NOT NULL,                                         -- OK
  `check_type` smallint(6) NOT NULL default '0',                         -- int
  -- check_options int
  `latency` double NOT NULL default '0',                                 -- float
  `execution_time` double NOT NULL default '0',                          -- float
  -- last_state int
  -- last_update int
  -- timeout int
  -- start_time int
  -- end_time int
  -- early_timeout smallint
  -- return_code smallint
  `last_state_change` int(11) NOT NULL,                                  -- OK
  `last_hard_state_change` int(11) NOT NULL,                             -- OK
  `last_hard_state` smallint(6) NOT NULL default '0',                    -- int, why ?
  `last_time_ok` int(11) NOT NULL,                                       -- OK
  `last_time_warning` int(11) NOT NULL,                                  -- OK
  `last_time_unknown` int(11) NOT NULL,                                  -- OK
  `last_time_critical` int(11) NOT NULL,                                 -- OK
  `default_passive_checks_enabled` smallint(6) NOT NULL default '0',
  `default_active_checks_enabled` smallint(6) NOT NULL default '0',
  `last_notification` int(11) NOT NULL,                                  -- OK
  `next_notification` int(11) NOT NULL,                                  -- OK
  `no_more_notifications` smallint(6) NOT NULL default '0',              -- int, why ?
  -- check_flapping_recovery_notification int
  `notifications_enabled` smallint(6) NOT NULL default '0',              -- tinyint(1)
  `default_notifications_enabled` smallint(6) NOT NULL default '0',
  `current_notification_number` smallint(6) NOT NULL default '0',        -- int
  -- current_notification_id int
  `first_notification_delay` double NOT NULL default '0',                -- int
  `notification_interval` double NOT NULL default '0',                   -- int
  -- notification_period
  -- notification_options
  -- notified_on_unknown int
  -- notified_on_warning int
  -- notified_on_critical int
  `notify_on_warning` smallint(6) NOT NULL default '0',
  `notify_on_unknown` smallint(6) NOT NULL default '0',
  `notify_on_critical` smallint(6) NOT NULL default '0',
  `notify_on_recovery` smallint(6) NOT NULL default '0',
  `notify_on_flapping` smallint(6) NOT NULL default '0',
  `notify_on_downtime` smallint(6) NOT NULL default '0',
  `problem_has_been_acknowledged` smallint(6) NOT NULL default '0',      -- int, why ?
  `acknowledgement_type` smallint(6) NOT NULL default '0',               -- int, why ?
  -- host_problem_at_last_check
  `flap_detection_enabled` smallint(6) NOT NULL default '0',             -- tinyint(1)
  -- flap_detection_options varchar(18)
  `default_flap_detection_enabled` smallint(6) NOT NULL default '0',
  `flap_detection_on_ok` smallint(6) NOT NULL default '0',
  `flap_detection_on_warning` smallint(6) NOT NULL default '0',
  `flap_detection_on_unknown` smallint(6) NOT NULL default '0',
  `flap_detection_on_critical` smallint(6) NOT NULL default '0',
  `is_flapping` smallint(6) NOT NULL default '0',                        -- int, why ?
  -- flapping_comment_id
  `percent_state_change` double NOT NULL default '0',                    -- float
  `low_flap_threshold` double NOT NULL default '0',                      -- float
  `high_flap_threshold` double NOT NULL default '0',                     -- float
  `scheduled_downtime_depth` smallint(6) NOT NULL default '0',           -- int
  -- pending_flex_downtime int
  `default_failure_prediction_enabled` smallint(6) NOT NULL default '0',
  -- process_perf_data tinyint(1)
  `process_performance_data` smallint(6) NOT NULL default '0',           -- int
  `default_process_performance_data` smallint(6) NOT NULL default '0',
  `obsess_over_service` smallint(6) NOT NULL default '0',                -- tinyint(1)
  `modified_attributes` int(11) NOT NULL default '0',                    -- OK
  `event_handler` varchar(255) NOT NULL default '',                      -- int
  -- event_handler_args text
  `check_command` varchar(255) NOT NULL default '',                      -- text
  -- stalking_options
  `stalk_on_ok` smallint(6) NOT NULL default '0',
  `stalk_on_warning` smallint(6) NOT NULL default '0',
  `stalk_on_unknown` smallint(6) NOT NULL default '0',
  `stalk_on_critical` smallint(6) NOT NULL default '0',
  `is_volatile` smallint(6) NOT NULL default '0',                        -- tinyint(1)
  `event_handler_enabled` smallint(6) NOT NULL default '0',              -- tinyint(1)
  `default_event_handler_enabled` smallint(6) NOT NULL default '0',
  -- check_freshness tinyint(1)
  `freshness_checks_enabled` smallint(6) NOT NULL default '0',
  `freshness_threshold` smallint(6) NOT NULL default '0',                -- int
  `retain_status_information` smallint(6) NOT NULL default '0',          -- tinyint(1)
  `retain_nonstatus_information` smallint(6) NOT NULL default '0',       -- tinyint(1)
  `failure_prediction_enabled` smallint(6) NOT NULL default '0',         -- tinyint(1)
  `failure_prediction_options` varchar(64) NOT NULL default '',
  `notes` varchar(255) default NULL,                                     -- OK
  `notes_url` varchar(255) default NULL,                                 -- OK
  `action_url` varchar(255) default NULL,                                -- OK
  `icon_image` varchar(255) default NULL,                                -- varchar(60)
  `icon_image_alt` varchar(255) default NULL,                            -- varchar(60)
  PRIMARY KEY  (`id`),
  UNIQUE KEY `key1` (`instance_id`, `host_name`, `service_description`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Service information';


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


CREATE TABLE IF NOT EXISTS `instances` (
  `id` int(11) NOT NULL auto_increment,
  `instance_name` varchar(64) NOT NULL default '',
  `instance_description` varchar(128) NOT NULL default '',
  `instance_address` varchar(120) default NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Location names of various Nagios installations';


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
