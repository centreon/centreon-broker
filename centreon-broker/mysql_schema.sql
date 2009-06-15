CREATE TABLE IF NOT EXISTS `acknowledgements` (
  `acknowledgement_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
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
  PRIMARY KEY  (`acknowledgement_id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Current and historical host and service acknowledgements' ;


CREATE TABLE IF NOT EXISTS `centreon_acl` (
  `id` int(11) NOT NULL auto_increment,
  `host_name` varchar(60) default NULL,
  `service_description` varchar(128) default NULL,
  `group_id` int(11) default NULL,
  `host_id` int(11) default NULL,
  `service_id` int(11) default NULL,
  PRIMARY KEY  (`id`),
  KEY `host_name` (`host_name`,`service_description`,`group_id`,`host_id`,`service_id`),
  KEY `host_name_2` (`host_name`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci ;


CREATE TABLE IF NOT EXISTS `comments` (
  `comment_id` int(11) NOT NULL auto_increment,
  `internal_comment_id` int(11) NOT NULL default '0',
  `host_id` smallint(5) unsigned default NULL,
  `service_id` smallint(5) unsigned default NULL,
  `instance_id` smallint(5) unsigned default NULL,
  `author_name` varchar(64) NOT NULL default '',
  `entry_time` int(11) NOT NULL,
  `comment_type` smallint(6) NOT NULL default '0',
  `entry_type` smallint(6) NOT NULL default '0',
  `comment_time` int(11) NOT NULL,
  `comment_data` varchar(255) NOT NULL default '',
  `is_persistent` smallint(6) NOT NULL default '0',
  `comment_source` smallint(6) NOT NULL default '0',
  `expires` smallint(6) NOT NULL default '0',
  `expiration_time` int(11) NOT NULL,
  `deletion_time` int(11) NOT NULL,
  PRIMARY KEY  (`comment_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `connection_info` (
  `conninfo_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
  `agent_name` varchar(32) NOT NULL default '',
  `agent_version` varchar(8) NOT NULL default '',
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
  PRIMARY KEY  (`conninfo_id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci ;


CREATE TABLE IF NOT EXISTS `customvariables` (
  `customvariable_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
  `object_id` int(11) NOT NULL default '0',
  `config_type` smallint(6) NOT NULL default '0',
  `has_been_modified` smallint(6) NOT NULL default '0',
  `varname` varchar(255) NOT NULL default '',
  `varvalue` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`customvariable_id`),
  UNIQUE KEY `object_id_2` (`object_id`,`config_type`,`varname`),
  KEY `varname` (`varname`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Custom variables' ;


CREATE TABLE IF NOT EXISTS `customvariablestatus` (
  `customvariablestatus_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
  `object_id` int(11) NOT NULL default '0',
  `status_update_time` int(11) NOT NULL,
  `has_been_modified` smallint(6) NOT NULL default '0',
  `varname` varchar(255) NOT NULL default '',
  `varvalue` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`customvariablestatus_id`),
  UNIQUE KEY `object_id_2` (`object_id`,`varname`),
  KEY `varname` (`varname`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Custom variable status information';


CREATE TABLE IF NOT EXISTS `downtimes` (
  `downtime_id` int(11) NOT NULL auto_increment,
  `host_id` smallint(5) unsigned default NULL,
  `service_id` smallint(5) unsigned default NULL,
  `instance_id` smallint(6) NOT NULL default '0',
  `downtime_type` smallint(6) NOT NULL default '0',
  `entry_time` int(11) NOT NULL,
  `author_name` varchar(64) NOT NULL default '',
  `comment_data` varchar(255) NOT NULL default '',
  `internal_downtime_id` int(11) NOT NULL default '0',
  `triggered_by_id` int(11) NOT NULL default '0',
  `is_fixed` smallint(6) NOT NULL default '0',
  `duration` smallint(6) NOT NULL default '0',
  `scheduled_start_time` int(11) NOT NULL,
  `scheduled_end_time` int(11) NOT NULL,
  `was_started` smallint(6) NOT NULL default '0',
  `actual_start_time` int(11) NOT NULL,
  `actual_start_time_usec` int(11) NOT NULL default '0',
  `actual_end_time` int(11) NOT NULL,
  `actual_end_time_usec` int(11) NOT NULL default '0',
  `was_cancelled` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`downtime_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci ;


CREATE TABLE IF NOT EXISTS `eventhandlers` (
  `eventhandler_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
  `host_id` smallint(5) unsigned default NULL,
  `service_id` smallint(5) unsigned default NULL,
  `eventhandler_type` smallint(6) NOT NULL default '0',
  `state` smallint(6) NOT NULL default '0',
  `state_type` smallint(6) NOT NULL default '0',
  `start_time` int(11) NOT NULL,
  `end_time` int(11) NOT NULL,
  `command_args` varchar(255) NOT NULL default '',
  `command_line` varchar(255) NOT NULL default '',
  `timeout` smallint(6) NOT NULL default '0',
  `early_timeout` smallint(6) NOT NULL default '0',
  `execution_time` double NOT NULL default '0',
  `return_code` smallint(6) NOT NULL default '0',
  `output` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`eventhandler_id`),
  UNIQUE KEY `instance_id` (`instance_id`,`start_time`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Historical host and service event handlers' ;


CREATE TABLE IF NOT EXISTS `flappinghistory` (
  `flappinghistory_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
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
  PRIMARY KEY  (`flappinghistory_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Current and historical record of host and service flapping' ;


CREATE TABLE IF NOT EXISTS `hostgroups` (
  `hostgroup_id` int(11) NOT NULL auto_increment,
  `alias` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `enabled` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`hostgroup_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Hostgroup definitions' ;


CREATE TABLE IF NOT EXISTS `hostgroups_members` (
  `hostgroup_id` smallint(5) unsigned NOT NULL default '0',
  `host_id` smallint(5) unsigned default NULL,
  `hostgroup_child_id` smallint(6) default NULL,
  PRIMARY KEY  (`hostgroup_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `hosts` (
  `host_id` int(11) NOT NULL auto_increment,
  `host_name` varchar(255) default NULL,
  `instance_id` smallint(6) NOT NULL default '0',
  `alias` varchar(64) NOT NULL default '',
  `display_name` varchar(64) NOT NULL default '',
  `address` varchar(128) NOT NULL default '',
  `check_interval` double NOT NULL default '0',
  `retry_interval` double NOT NULL default '0',
  `max_check_attempts` smallint(6) NOT NULL default '0',
  `first_notification_delay` double NOT NULL default '0',
  `notification_interval` double NOT NULL default '0',
  `status_update_time` int(11) NOT NULL,
  `output` varchar(255) NOT NULL default '',
  `perfdata` varchar(255) NOT NULL default '',
  `current_state` smallint(6) NOT NULL default '0',
  `has_been_checked` smallint(6) NOT NULL default '0',
  `should_be_scheduled` smallint(6) NOT NULL default '0',
  `current_check_attempt` smallint(6) NOT NULL default '0',
  `last_check` int(11) NOT NULL,
  `next_check` int(11) NOT NULL,
  `check_type` smallint(6) NOT NULL default '0',
  `last_state_change` int(11) NOT NULL,
  `last_hard_state_change` int(11) NOT NULL,
  `last_hard_state` smallint(6) NOT NULL default '0',
  `last_time_up` int(11) NOT NULL,
  `last_time_down` int(11) NOT NULL,
  `last_time_unreachable` int(11) NOT NULL,
  `state_type` smallint(6) NOT NULL default '0',
  `last_notification` int(11) NOT NULL,
  `next_notification` int(11) NOT NULL,
  `no_more_notifications` smallint(6) NOT NULL default '0',
  `notifications_enabled` smallint(6) NOT NULL default '0',
  `problem_has_been_acknowledged` smallint(6) NOT NULL default '0',
  `acknowledgement_type` smallint(6) NOT NULL default '0',
  `current_notification_number` smallint(6) NOT NULL default '0',
  `passive_checks_enabled` smallint(6) NOT NULL default '0',
  `active_checks_enabled` smallint(6) NOT NULL default '0',
  `event_handler_enabled` smallint(6) NOT NULL default '0',
  `flap_detection_enabled` smallint(6) NOT NULL default '0',
  `is_flapping` smallint(6) NOT NULL default '0',
  `percent_state_change` double NOT NULL default '0',
  `latency` double NOT NULL default '0',
  `execution_time` double NOT NULL default '0',
  `scheduled_downtime_depth` smallint(6) NOT NULL default '0',
  `failure_prediction_enabled` smallint(6) NOT NULL default '0',
  `process_performance_data` smallint(6) NOT NULL default '0',
  `obsess_over_host` smallint(6) NOT NULL default '0',
  `modified_host_attributes` int(11) NOT NULL default '0',
  `event_handler` varchar(255) NOT NULL default '',
  `check_command` varchar(255) NOT NULL default '',
  `notes` varchar(255) NOT NULL default '',
  `notes_url` varchar(255) NOT NULL default '',
  `action_url` varchar(255) NOT NULL default '',
  `icon_image` varchar(255) NOT NULL default '',
  `icon_image_alt` varchar(255) NOT NULL default '',
  `vrml_image` varchar(255) NOT NULL default '',
  `statusmap_image` varchar(255) NOT NULL default '',
  `have_2d_coords` smallint(6) NOT NULL default '0',
  `x_2d` smallint(6) NOT NULL default '0',
  `y_2d` smallint(6) NOT NULL default '0',
  `notify_on_down` smallint(6) NOT NULL default '0',
  `notify_on_unreachable` smallint(6) NOT NULL default '0',
  `notify_on_recovery` smallint(6) NOT NULL default '0',
  `notify_on_flapping` smallint(6) NOT NULL default '0',
  `notify_on_downtime` smallint(6) NOT NULL default '0',
  `stalk_on_up` smallint(6) NOT NULL default '0',
  `stalk_on_down` smallint(6) NOT NULL default '0',
  `stalk_on_unreachable` smallint(6) NOT NULL default '0',
  `flap_detection_on_up` smallint(6) NOT NULL default '0',
  `flap_detection_on_down` smallint(6) NOT NULL default '0',
  `flap_detection_on_unreachable` smallint(6) NOT NULL default '0',
  `low_flap_threshold` double NOT NULL default '0',
  `high_flap_threshold` double NOT NULL default '0',
  `freshness_threshold` smallint(6) NOT NULL default '0',
  `retain_status_information` smallint(6) NOT NULL default '0',
  `retain_nonstatus_information` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`host_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Host information';


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
  `instance_id` smallint(6) NOT NULL auto_increment,
  `instance_name` varchar(64) NOT NULL default '',
  `instance_description` varchar(128) NOT NULL default '',
  `instance_address` varchar(120) default NULL,
  PRIMARY KEY  (`instance_id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Location names of various Nagios installations';


CREATE TABLE IF NOT EXISTS `processevents` (
  `processevent_id` int(11) NOT NULL auto_increment,
  `instance_id` smallint(6) NOT NULL default '0',
  `event_type` smallint(6) NOT NULL default '0',
  `event_time` int(11) NOT NULL,
  `event_time_usec` int(11) NOT NULL default '0',
  `process_id` int(11) NOT NULL default '0',
  `program_name` varchar(16) NOT NULL default '',
  `program_version` varchar(20) NOT NULL default '',
  `program_date` varchar(10) NOT NULL default '',
  PRIMARY KEY  (`processevent_id`)
) ENGINE=InnoDB  DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Historical Nagios process events';


CREATE TABLE IF NOT EXISTS `programstatus` (
  `instance_id` smallint(6) NOT NULL default '0',
  `status_update_time` int(11) NOT NULL,
  `program_start_time` int(11) NOT NULL,
  `program_end_time` int(11) NOT NULL,
  `is_currently_running` smallint(6) NOT NULL default '0',
  `process_id` int(11) NOT NULL default '0',
  `daemon_mode` smallint(6) NOT NULL default '0',
  `last_command_check` int(11) NOT NULL,
  `last_log_rotation` int(11) NOT NULL,
  `notifications_enabled` smallint(6) NOT NULL default '0',
  `active_service_checks_enabled` smallint(6) NOT NULL default '0',
  `passive_service_checks_enabled` smallint(6) NOT NULL default '0',
  `active_host_checks_enabled` smallint(6) NOT NULL default '0',
  `passive_host_checks_enabled` smallint(6) NOT NULL default '0',
  `event_handlers_enabled` smallint(6) NOT NULL default '0',
  `flap_detection_enabled` smallint(6) NOT NULL default '0',
  `failure_prediction_enabled` smallint(6) NOT NULL default '0',
  `process_performance_data` smallint(6) NOT NULL default '0',
  `obsess_over_hosts` smallint(6) NOT NULL default '0',
  `obsess_over_services` smallint(6) NOT NULL default '0',
  `modified_host_attributes` int(11) NOT NULL default '0',
  `modified_service_attributes` int(11) NOT NULL default '0',
  `global_host_event_handler` varchar(255) NOT NULL default '',
  `global_service_event_handler` varchar(255) NOT NULL default '',
  UNIQUE KEY `instance_id` (`instance_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `servicegroups` (
  `servicegroup_id` int(11) NOT NULL auto_increment,
  `alias` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `enabled` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`servicegroup_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='ServiceGroup definitions' ;


CREATE TABLE IF NOT EXISTS `servicegroups_members` (
  `servicegroup_id` smallint(5) unsigned NOT NULL default '0',
  `service_id` smallint(5) unsigned default NULL,
  `servicegroup_child_id` smallint(5) unsigned default NULL,
  PRIMARY KEY  (`servicegroup_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `services` (
  `host_id` int(11) NOT NULL,
  `service_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_name` varchar(64) NOT NULL default '',
  `service_description` varchar(64) NOT NULL default '',
  `instance_id` smallint(6) NOT NULL default '0',
  `display_name` varchar(64) NOT NULL default '',
  `graph_id` int(11) NOT NULL,
  `status_update_time` int(11) NOT NULL,
  `check_interval` double NOT NULL default '0',
  `retry_interval` double NOT NULL default '0',
  `max_check_attempts` smallint(6) NOT NULL default '0',
  `current_check_attempt` smallint(6) NOT NULL default '0',
  `has_been_checked` smallint(6) NOT NULL default '0',
  `should_be_scheduled` smallint(6) NOT NULL default '0',
  `passive_checks_enabled` smallint(6) NOT NULL default '0',
  `active_checks_enabled` smallint(6) NOT NULL default '0',
  `current_state` smallint(6) NOT NULL default '0',
  `state_type` smallint(6) NOT NULL default '0',
  `output` varchar(255) NOT NULL default '',
  `perfdata` varchar(255) NOT NULL default '',
  `last_check` int(11) NOT NULL,
  `next_check` int(11) NOT NULL,
  `check_type` smallint(6) NOT NULL default '0',
  `latency` double NOT NULL default '0',
  `execution_time` double NOT NULL default '0',
  `last_state_change` int(11) NOT NULL,
  `last_hard_state_change` int(11) NOT NULL,
  `last_hard_state` smallint(6) NOT NULL default '0',
  `last_time_ok` int(11) NOT NULL,
  `last_time_warning` int(11) NOT NULL,
  `last_time_unknown` int(11) NOT NULL,
  `last_time_critical` int(11) NOT NULL,
  `default_passive_checks_enabled` smallint(6) NOT NULL default '0',
  `default_active_checks_enabled` smallint(6) NOT NULL default '0',
  `last_notification` int(11) NOT NULL,
  `next_notification` int(11) NOT NULL,
  `no_more_notifications` smallint(6) NOT NULL default '0',
  `notifications_enabled` smallint(6) NOT NULL default '0',
  `default_notifications_enabled` smallint(6) NOT NULL default '0',
  `current_notification_number` smallint(6) NOT NULL default '0',
  `first_notification_delay` double NOT NULL default '0',
  `notification_interval` double NOT NULL default '0',
  `notify_on_warning` smallint(6) NOT NULL default '0',
  `notify_on_unknown` smallint(6) NOT NULL default '0',
  `notify_on_critical` smallint(6) NOT NULL default '0',
  `notify_on_recovery` smallint(6) NOT NULL default '0',
  `notify_on_flapping` smallint(6) NOT NULL default '0',
  `notify_on_downtime` smallint(6) NOT NULL default '0',
  `problem_has_been_acknowledged` smallint(6) NOT NULL default '0',
  `acknowledgement_type` smallint(6) NOT NULL default '0',
  `flap_detection_enabled` smallint(6) NOT NULL default '0',
  `default_flap_detection_enabled` smallint(6) NOT NULL default '0',
  `flap_detection_on_ok` smallint(6) NOT NULL default '0',
  `flap_detection_on_warning` smallint(6) NOT NULL default '0',
  `flap_detection_on_unknown` smallint(6) NOT NULL default '0',
  `flap_detection_on_critical` smallint(6) NOT NULL default '0',
  `is_flapping` smallint(6) NOT NULL default '0',
  `percent_state_change` double NOT NULL default '0',
  `low_flap_threshold` double NOT NULL default '0',
  `high_flap_threshold` double NOT NULL default '0',
  `scheduled_downtime_depth` smallint(6) NOT NULL default '0',
  `default_failure_prediction_enabled` smallint(6) NOT NULL default '0',
  `process_performance_data` smallint(6) NOT NULL default '0',
  `default_process_performance_data` smallint(6) NOT NULL default '0',
  `obsess_over_service` smallint(6) NOT NULL default '0',
  `modified_service_attributes` int(11) NOT NULL default '0',
  `event_handler` varchar(255) NOT NULL default '',
  `check_command` varchar(255) NOT NULL default '',
  `stalk_on_ok` smallint(6) NOT NULL default '0',
  `stalk_on_warning` smallint(6) NOT NULL default '0',
  `stalk_on_unknown` smallint(6) NOT NULL default '0',
  `stalk_on_critical` smallint(6) NOT NULL default '0',
  `is_volatile` smallint(6) NOT NULL default '0',
  `event_handler_enabled` smallint(6) NOT NULL default '0',
  `default_event_handler_enabled` smallint(6) NOT NULL default '0',
  `freshness_checks_enabled` smallint(6) NOT NULL default '0',
  `freshness_threshold` smallint(6) NOT NULL default '0',
  `retain_status_information` smallint(6) NOT NULL default '0',
  `retain_nonstatus_information` smallint(6) NOT NULL default '0',
  `failure_prediction_enabled` smallint(6) NOT NULL default '0',
  `failure_prediction_options` varchar(64) NOT NULL default '',
  `notes` varchar(255) NOT NULL default '',
  `notes_url` varchar(255) NOT NULL default '',
  `action_url` varchar(255) NOT NULL default '',
  `icon_image` varchar(255) NOT NULL default '',
  `icon_image_alt` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`service_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT='Service information';


CREATE TABLE IF NOT EXISTS `services_commands` (
  `service_id` smallint(5) unsigned NOT NULL default '0',
  `check_command` text,
  `event_handler_command` text,
  PRIMARY KEY  (`service_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


CREATE TABLE IF NOT EXISTS `services_longouput` (
  `service_id` smallint(5) unsigned NOT NULL default '0',
  `longouput` text NOT NULL,
  PRIMARY KEY  (`service_id`)
) ENGINE=InnoDB DEFAULT  CHARACTER SET utf8 COLLATE utf8_general_ci;


