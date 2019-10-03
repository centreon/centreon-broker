/*
** Copyright 1999-2009 Ethan Galstad
** Copyright 2009-2010 Nagios Core Development Team and Community Contributors
** Copyright 2011-2019 Centreon
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCE_GLOBALS_HH
#define CCE_GLOBALS_HH

#include <stdio.h>
#include <map>
#include <string>
#include "com/centreon/engine/checks.hh"
#include "com/centreon/engine/circular_buffer.hh"
#include "com/centreon/engine/configuration/state.hh"
#include "com/centreon/engine/downtimes/downtime.hh"
#include "com/centreon/engine/events/sched_info.hh"
#include "com/centreon/engine/events/timed_event.hh"
#include "com/centreon/engine/nebmods.hh"
#include "com/centreon/engine/utils.hh"

#ifdef __cplusplus
extern "C" {
#endif /* C++ */

extern int config_errors;
extern int config_warnings;

extern char* check_result_path;

extern com::centreon::engine::configuration::state* config;
extern char* config_file;

extern com::centreon::engine::commands::command* global_host_event_handler_ptr;
extern com::centreon::engine::commands::command*
    global_service_event_handler_ptr;

extern com::centreon::engine::commands::command* ocsp_command_ptr;
extern com::centreon::engine::commands::command* ochp_command_ptr;

extern unsigned long logging_options;

extern time_t last_command_check;
extern time_t last_command_status_update;
extern time_t last_log_rotation;

extern unsigned long modified_host_process_attributes;
extern unsigned long modified_service_process_attributes;

extern unsigned long next_event_id;
extern unsigned long next_problem_id;
extern unsigned long next_notification_id;

extern bool sighup;
extern int sigshutdown;
extern int sigrestart;

extern char const* sigs[35];

extern int sig_id;

extern int verify_config;
extern int verify_circular_paths;
extern int test_scheduling;

extern uint32_t currently_running_service_checks;
extern uint32_t currently_running_host_checks;

extern time_t program_start;
extern time_t event_start;

extern circular_buffer external_command_buffer;
extern pthread_t worker_threads[];

extern check_stats check_statistics[];

extern sched_info scheduling_info;

extern std::string macro_x_names[];
extern std::string macro_user[];

extern nebcallback* neb_callback_list[];

extern char* log_file;
extern char* debug_file;
extern char* global_host_event_handler;
extern char* global_service_event_handler;
extern char* ocsp_command;
extern char* ochp_command;
extern uint32_t log_notifications;
extern uint32_t log_passive_checks;
extern int additional_freshness_latency;
extern uint32_t obsess_over_services;
extern uint32_t obsess_over_hosts;
extern uint32_t notification_timeout;
extern uint32_t use_aggressive_host_checking;
extern unsigned long cached_host_check_horizon;
extern uint32_t soft_state_dependencies;
extern uint32_t enable_event_handlers;
extern uint32_t enable_notifications;
extern uint32_t execute_service_checks;
extern uint32_t accept_passive_service_checks;
extern uint32_t execute_host_checks;
extern uint32_t accept_passive_host_checks;
extern uint32_t max_service_check_spread;
extern uint32_t max_host_check_spread;
extern uint32_t check_reaper_interval;
extern uint32_t interval_length;
extern uint32_t check_external_commands;
extern uint32_t check_service_freshness;
extern uint32_t check_host_freshness;
extern uint32_t process_performance_data;
extern uint32_t enable_flap_detection;
extern char* use_timezone;
extern char* illegal_object_chars;
extern char* illegal_output_chars;
extern uint32_t use_large_installation_tweaks;

#ifdef __cplusplus
}
#endif /* C++ */

#endif /* !CCE_GLOBALS_HH */
