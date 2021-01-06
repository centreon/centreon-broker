/*
** Copyright 2009-2020 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/neb/callbacks.hh"

#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <memory>
#include <set>
#include <string>

#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/neb/callback.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/initial.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/set_log_data.hh"
#include "com/centreon/broker/neb/statistics/generator.hh"
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/comment.hh"
#include "com/centreon/engine/events/loop.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/hostdependency.hh"
#include "com/centreon/engine/hostgroup.hh"
#include "com/centreon/engine/nebcallbacks.hh"
#include "com/centreon/engine/nebstructs.hh"
#include "com/centreon/engine/servicedependency.hh"
#include "com/centreon/engine/servicegroup.hh"

using namespace com::centreon::broker;

// List of Nagios modules.
extern nebmodule* neb_module_list;

// Acknowledgement list.
std::map<std::pair<uint32_t, uint32_t>, neb::acknowledgement>
    neb::gl_acknowledgements;

// Downtime list.
struct private_downtime_params {
  bool cancelled;
  time_t deletion_time;
  time_t end_time;
  bool started;
  time_t start_time;
};
// Unstarted downtimes.
static std::unordered_map<uint32_t, private_downtime_params> downtimes;

// Load flags.
int neb::gl_mod_flags(0);

// Module handle.
void* neb::gl_mod_handle(nullptr);

// List of common callbacks.
static struct {
  uint32_t macro;
  int (*callback)(int, void*);
} const gl_callbacks[] = {
    {NEBCALLBACK_ACKNOWLEDGEMENT_DATA, &neb::callback_acknowledgement},
    {NEBCALLBACK_COMMENT_DATA, &neb::callback_comment},
    {NEBCALLBACK_DOWNTIME_DATA, &neb::callback_downtime},
    {NEBCALLBACK_EVENT_HANDLER_DATA, &neb::callback_event_handler},
    {NEBCALLBACK_EXTERNAL_COMMAND_DATA, &neb::callback_external_command},
    {NEBCALLBACK_FLAPPING_DATA, &neb::callback_flapping_status},
    {NEBCALLBACK_HOST_CHECK_DATA, &neb::callback_host_check},
    {NEBCALLBACK_HOST_STATUS_DATA, &neb::callback_host_status},
    {NEBCALLBACK_PROGRAM_STATUS_DATA, &neb::callback_program_status},
    {NEBCALLBACK_SERVICE_CHECK_DATA, &neb::callback_service_check},
    {NEBCALLBACK_SERVICE_STATUS_DATA, &neb::callback_service_status}};

// List of Engine-specific callbacks.
static struct {
  uint32_t macro;
  int (*callback)(int, void*);
} const gl_engine_callbacks[] = {
    {NEBCALLBACK_ADAPTIVE_DEPENDENCY_DATA, &neb::callback_dependency},
    {NEBCALLBACK_ADAPTIVE_HOST_DATA, &neb::callback_host},
    {NEBCALLBACK_ADAPTIVE_SERVICE_DATA, &neb::callback_service},
    {NEBCALLBACK_CUSTOM_VARIABLE_DATA, &neb::callback_custom_variable},
    {NEBCALLBACK_GROUP_DATA, &neb::callback_group},
    {NEBCALLBACK_GROUP_MEMBER_DATA, &neb::callback_group_member},
    {NEBCALLBACK_MODULE_DATA, &neb::callback_module},
    {NEBCALLBACK_RELATION_DATA, &neb::callback_relation}};

// Registered callbacks.
std::list<std::shared_ptr<neb::callback> > neb::gl_registered_callbacks;

// External function to get program version.
extern "C" {
char const* get_program_version();
}

// Statistics generator.
static neb::statistics::generator gl_generator;

extern "C" void event_statistics(void* args) {
  (void)args;
  try {
    gl_generator.run();
  } catch (std::exception const& e) {
    logging::error(logging::medium)
        << "stats: error occurred while generating statistics event: "
        << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
}

/**
 *  @brief Function that process acknowledgement data.
 *
 *  This function is called by Nagios when some acknowledgement data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ACKNOWLEDGEMENT_DATA).
 *  @param[in] data          A pointer to a nebstruct_acknowledgement_data
 *                           containing the acknowledgement data.
 *
 *  @return 0 on success.
 */
int neb::callback_acknowledgement(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium)
      << "callbacks: generating acknowledgement event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_acknowledgement_data const* ack_data;
    std::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);

    // Fill output var.
    ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
    ack->acknowledgement_type = ack_data->acknowledgement_type;
    if (ack_data->author_name)
      ack->author = misc::string::check_string_utf8(ack_data->author_name);
    if (ack_data->comment_data)
      ack->comment = misc::string::check_string_utf8(ack_data->comment_data);
    ack->entry_time = time(nullptr);
    if (!ack_data->host_id)
      throw exceptions::msg() << "unnamed host";
    if (ack_data->service_id) {
      ack->host_id = ack_data->host_id;
      ack->service_id = ack_data->service_id;
      if (!ack->host_id || !ack->service_id)
        throw exceptions::msg()
            << "acknowledgement on service with host_id or service_id 0";
    } else {
      ack->host_id = ack_data->host_id;
      if (ack->host_id == 0)
        throw exceptions::msg() << "acknowledgement on host with id 0";
    }
    ack->poller_id = config::applier::state::instance().poller_id();
    ack->is_sticky = ack_data->is_sticky;
    ack->notify_contacts = ack_data->notify_contacts;
    ack->persistent_comment = ack_data->persistent_comment;
    ack->state = ack_data->state;
    gl_acknowledgements[std::make_pair(ack->host_id, ack->service_id)] = *ack;

    // Send event.
    gl_publisher.write(ack);
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating acknowledgement event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process comment data.
 *
 *  This function is called by Nagios when some comment data are available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_COMMENT_DATA).
 *  @param[in] data          A pointer to a nebstruct_comment_data containing
 *                           the comment data.
 *
 *  @return 0 on success.
 */
int neb::callback_comment(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating comment event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_comment_data const* comment_data;
    std::shared_ptr<neb::comment> comment(new neb::comment);

    // Fill output var.
    comment_data = static_cast<nebstruct_comment_data*>(data);
    if (comment_data->author_name)
      comment->author =
          misc::string::check_string_utf8(comment_data->author_name);
    if (comment_data->comment_data)
      comment->data =
          misc::string::check_string_utf8(comment_data->comment_data);
    comment->comment_type = comment_data->comment_type;
    if (NEBTYPE_COMMENT_DELETE == comment_data->type)
      comment->deletion_time = time(nullptr);
    comment->entry_time = comment_data->entry_time;
    comment->entry_type = comment_data->entry_type;
    comment->expire_time = comment_data->expire_time;
    comment->expires = comment_data->expires;
    if (comment_data->service_id) {
      comment->host_id = comment_data->host_id;
      comment->service_id = comment_data->service_id;
      if (!comment->host_id || !comment->service_id)
        throw exceptions::msg()
            << "comment created from a service with host_id/service_id 0";
    } else {
      comment->host_id = comment_data->host_id;
      if (comment->host_id == 0)
        throw exceptions::msg() << "comment created from a host with host_id 0";
    }
    comment->poller_id = config::applier::state::instance().poller_id();
    comment->internal_id = comment_data->comment_id;
    comment->persistent = comment_data->persistent;
    comment->source = comment_data->source;

    // Send event.
    gl_publisher.write(comment);
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating comment event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process custom variable data.
 *
 *  This function is called by Engine when some custom variable data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_CUSTOMVARIABLE_DATA).
 *  @param[in] data          Pointer to a nebstruct_custom_variable_data
 *                           containing the custom variable data.
 *
 *  @return 0 on success.
 */
int neb::callback_custom_variable(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium)
      << "callbacks: generating custom variable event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_custom_variable_data const* cvar(
        static_cast<nebstruct_custom_variable_data*>(data));
    if (cvar && cvar->var_name && cvar->var_value) {
      // Host custom variable.
      if (NEBTYPE_HOSTCUSTOMVARIABLE_ADD == cvar->type) {
        engine::host* hst(static_cast<engine::host*>(cvar->object_ptr));
        if (hst && !hst->get_name().empty()) {
          // Fill custom variable event.
          uint64_t host_id = engine::get_host_id(hst->get_name());
          if (host_id != 0) {
            std::shared_ptr<custom_variable> new_cvar(new custom_variable);
            new_cvar->enabled = true;
            new_cvar->host_id = host_id;
            new_cvar->modified = false;
            new_cvar->name = misc::string::check_string_utf8(cvar->var_name);
            new_cvar->var_type = 0;
            new_cvar->update_time = cvar->timestamp.tv_sec;
            new_cvar->value = misc::string::check_string_utf8(cvar->var_value);
            new_cvar->default_value =
                misc::string::check_string_utf8(cvar->var_value);

            // Send custom variable event.
            logging::info(logging::low)
                << "callbacks: new custom variable '" << new_cvar->name
                << "' on host " << new_cvar->host_id;
            neb::gl_publisher.write(new_cvar);
          }
        }
      } else if (NEBTYPE_HOSTCUSTOMVARIABLE_DELETE == cvar->type) {
        engine::host* hst(static_cast<engine::host*>(cvar->object_ptr));
        if (hst && !hst->get_name().empty()) {
          uint32_t host_id = engine::get_host_id(hst->get_name());
          if (host_id != 0) {
            std::shared_ptr<custom_variable> old_cvar(new custom_variable);
            old_cvar->enabled = false;
            old_cvar->host_id = host_id;
            old_cvar->name = misc::string::check_string_utf8(cvar->var_name);
            old_cvar->var_type = 0;
            old_cvar->update_time = cvar->timestamp.tv_sec;

            // Send custom variable event.
            logging::info(logging::low)
                << "callbacks: deleted custom variable '" << old_cvar->name
                << "' on host '" << old_cvar->host_id;
            neb::gl_publisher.write(old_cvar);
          }
        }
      }
      // Service custom variable.
      else if (NEBTYPE_SERVICECUSTOMVARIABLE_ADD == cvar->type) {
        engine::service* svc{static_cast<engine::service*>(cvar->object_ptr)};
        if (svc && !svc->get_description().empty() &&
            !svc->get_hostname().empty()) {
          // Fill custom variable event.
          std::pair<uint32_t, uint32_t> p;
          p = engine::get_host_and_service_id(svc->get_hostname(),
                                              svc->get_description());
          if (p.first && p.second) {
            std::shared_ptr<custom_variable> new_cvar(new custom_variable);
            new_cvar->enabled = true;
            new_cvar->host_id = p.first;
            new_cvar->modified = false;
            new_cvar->name = misc::string::check_string_utf8(cvar->var_name);
            new_cvar->service_id = p.second;
            new_cvar->var_type = 1;
            new_cvar->update_time = cvar->timestamp.tv_sec;
            new_cvar->value = misc::string::check_string_utf8(cvar->var_value);
            new_cvar->default_value =
                misc::string::check_string_utf8(cvar->var_value);

            // Send custom variable event.
            logging::info(logging::low)
                << "callbacks: new custom variable '" << new_cvar->name
                << "' on service (" << new_cvar->host_id << ", "
                << new_cvar->service_id << ")";
            neb::gl_publisher.write(new_cvar);
          }
        }
      } else if (NEBTYPE_SERVICECUSTOMVARIABLE_DELETE == cvar->type) {
        engine::service* svc{static_cast<engine::service*>(cvar->object_ptr)};
        if (svc && !svc->get_description().empty() &&
            !svc->get_hostname().empty()) {
          std::pair<uint64_t, uint64_t> p{engine::get_host_and_service_id(
              svc->get_hostname(), svc->get_description())};
          if (p.first && p.second) {
            std::shared_ptr<custom_variable> old_cvar(new custom_variable);
            old_cvar->enabled = false;
            old_cvar->host_id = p.first;
            old_cvar->modified = true;
            old_cvar->name = misc::string::check_string_utf8(cvar->var_name);
            old_cvar->service_id = p.second;
            old_cvar->var_type = 1;
            old_cvar->update_time = cvar->timestamp.tv_sec;

            // Send custom variable event.
            logging::info(logging::low)
                << "callbacks: deleted custom variable '" << old_cvar->name
                << "' on service (" << old_cvar->host_id << ", "
                << old_cvar->service_id << ")";
            neb::gl_publisher.write(old_cvar);
          }
        }
      }
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process dependency data.
 *
 *  This function is called by Centreon Engine when some dependency data
 *  is available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ADAPTIVE_DEPENDENCY_DATA).
 *  @param[in] data          A pointer to a
 *                           nebstruct_adaptive_dependency_data
 *                           containing the dependency data.
 *
 *  @return 0 on success.
 */
int neb::callback_dependency(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating dependency event";
  (void)callback_type;

  try {
    // Input variables.
    nebstruct_adaptive_dependency_data* nsadd(
        static_cast<nebstruct_adaptive_dependency_data*>(data));

    // Host dependency.
    if ((NEBTYPE_HOSTDEPENDENCY_ADD == nsadd->type) ||
        (NEBTYPE_HOSTDEPENDENCY_UPDATE == nsadd->type) ||
        (NEBTYPE_HOSTDEPENDENCY_DELETE == nsadd->type)) {
      // Find IDs.
      uint64_t host_id;
      uint64_t dep_host_id;
      engine::hostdependency* dep(
          static_cast<engine::hostdependency*>(nsadd->object_ptr));
      if (!dep->get_hostname().empty()) {
        host_id = engine::get_host_id(dep->get_hostname());
      } else {
        logging::error(logging::medium)
            << "callbacks: dependency callback called without "
            << "valid host";
        host_id = 0;
      }
      if (!dep->get_dependent_hostname().empty()) {
        dep_host_id = engine::get_host_id(dep->get_dependent_hostname());
      } else {
        logging::error(logging::medium)
            << "callbacks: dependency callback called without "
            << "valid dependent host";
        dep_host_id = 0;
      }

      // Generate service dependency event.
      std::shared_ptr<host_dependency> hst_dep(new host_dependency);
      hst_dep->host_id = host_id;
      hst_dep->dependent_host_id = dep_host_id;
      hst_dep->enabled = (nsadd->type != NEBTYPE_HOSTDEPENDENCY_DELETE);
      if (!dep->get_dependency_period().empty())
        hst_dep->dependency_period = dep->get_dependency_period();
      {
        std::string options;
        if (dep->get_fail_on_down())
          options.append("d");
        if (dep->get_fail_on_up())
          options.append("o");
        if (dep->get_fail_on_pending())
          options.append("p");
        if (dep->get_fail_on_unreachable())
          options.append("u");
        if (dep->get_dependency_type() == engine::dependency::notification)
          hst_dep->notification_failure_options = options;
        else if (dep->get_dependency_type() == engine::dependency::execution)
          hst_dep->execution_failure_options = options;
      }
      hst_dep->inherits_parent = dep->get_inherits_parent();
      logging::info(logging::low) << "callbacks: host " << dep_host_id
                                  << " depends on host " << host_id;

      // Publish dependency event.
      neb::gl_publisher.write(hst_dep);
    }
    // Service dependency.
    else if ((NEBTYPE_SERVICEDEPENDENCY_ADD == nsadd->type) ||
             (NEBTYPE_SERVICEDEPENDENCY_UPDATE == nsadd->type) ||
             (NEBTYPE_SERVICEDEPENDENCY_DELETE == nsadd->type)) {
      // Find IDs.
      std::pair<uint64_t, uint64_t> ids;
      std::pair<uint64_t, uint64_t> dep_ids;
      engine::servicedependency* dep(
          static_cast<engine::servicedependency*>(nsadd->object_ptr));
      if (!dep->get_hostname().empty() &&
          !dep->get_service_description().empty()) {
        ids = engine::get_host_and_service_id(dep->get_hostname(),
                                              dep->get_service_description());
      } else {
        logging::error(logging::medium)
            << "callbacks: dependency callback called without "
            << "valid service";
        ids.first = 0;
        ids.second = 0;
      }
      if (!dep->get_dependent_hostname().empty() &&
          !dep->get_dependent_service_description().empty()) {
        dep_ids = engine::get_host_and_service_id(
            dep->get_hostname(), dep->get_service_description());
      } else {
        logging::error(logging::medium)
            << "callbacks: dependency callback called without "
            << "valid dependent service";
        dep_ids.first = 0;
        dep_ids.second = 0;
      }

      // Generate service dependency event.
      std::shared_ptr<service_dependency> svc_dep(new service_dependency);
      svc_dep->host_id = ids.first;
      svc_dep->service_id = ids.second;
      svc_dep->dependent_host_id = dep_ids.first;
      svc_dep->dependent_service_id = dep_ids.second;
      svc_dep->enabled = (nsadd->type != NEBTYPE_SERVICEDEPENDENCY_DELETE);
      if (!dep->get_dependency_period().empty())
        svc_dep->dependency_period = dep->get_dependency_period();
      {
        std::string options;
        if (dep->get_fail_on_critical())
          options.append("c");
        if (dep->get_fail_on_ok())
          options.append("o");
        if (dep->get_fail_on_pending())
          options.append("p");
        if (dep->get_fail_on_unknown())
          options.append("u");
        if (dep->get_fail_on_warning())
          options.append("w");
        if (dep->get_dependency_type() == engine::dependency::notification)
          svc_dep->notification_failure_options = options;
        else if (dep->get_dependency_type() == engine::dependency::execution)
          svc_dep->execution_failure_options = options;
      }
      svc_dep->inherits_parent = dep->get_inherits_parent();
      logging::info(logging::low)
          << "callbacks: service (" << dep_ids.first << ", " << dep_ids.second
          << ") depends on service (" << ids.first << ", " << ids.second << ")";

      // Publish dependency event.
      neb::gl_publisher.write(svc_dep);
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }

  return 0;
}

/**
 *  @brief Function that process downtime data.
 *
 *  This function is called by Nagios when some downtime data are available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_DOWNTIME_DATA).
 *  @param[in] data          A pointer to a nebstruct_downtime_data containing
 *                           the downtime data.
 *
 *  @return 0 on success.
 */
int neb::callback_downtime(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating downtime event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_downtime_data const* downtime_data;
    std::shared_ptr<neb::downtime> downtime(new neb::downtime);

    // Fill output var.
    downtime_data = static_cast<nebstruct_downtime_data*>(data);
    if (downtime_data->author_name)
      downtime->author =
          misc::string::check_string_utf8(downtime_data->author_name);
    if (downtime_data->comment_data)
      downtime->comment =
          misc::string::check_string_utf8(downtime_data->comment_data);
    downtime->downtime_type = downtime_data->downtime_type;
    downtime->duration = downtime_data->duration;
    downtime->end_time = downtime_data->end_time;
    downtime->entry_time = downtime_data->entry_time;
    downtime->fixed = downtime_data->fixed;
    if (!downtime_data->host_name)
      throw(exceptions::msg() << "unnamed host");
    if (downtime_data->service_description) {
      std::pair<uint32_t, uint32_t> p;
      p = engine::get_host_and_service_id(downtime_data->host_name,
                                          downtime_data->service_description);
      downtime->host_id = p.first;
      downtime->service_id = p.second;
      if (!downtime->host_id || !downtime->service_id)
        throw(exceptions::msg()
              << "could not find ID of service ('" << downtime_data->host_name
              << "', '" << downtime_data->service_description << "')");
    } else {
      downtime->host_id = engine::get_host_id(downtime_data->host_name);
      if (downtime->host_id == 0)
        throw(exceptions::msg() << "could not find ID of host '"
                                << downtime_data->host_name << "'");
    }
    downtime->poller_id = config::applier::state::instance().poller_id();
    downtime->internal_id = downtime_data->downtime_id;
    downtime->start_time = downtime_data->start_time;
    downtime->triggered_by = downtime_data->triggered_by;
    private_downtime_params& params(downtimes[downtime->internal_id]);
    if ((NEBTYPE_DOWNTIME_ADD == downtime_data->type) ||
        (NEBTYPE_DOWNTIME_LOAD == downtime_data->type)) {
      params.cancelled = false;
      params.deletion_time = -1;
      params.end_time = -1;
      params.started = false;
      params.start_time = -1;
    } else if (NEBTYPE_DOWNTIME_START == downtime_data->type) {
      params.started = true;
      params.start_time = downtime_data->timestamp.tv_sec;
    } else if (NEBTYPE_DOWNTIME_STOP == downtime_data->type) {
      if (NEBATTR_DOWNTIME_STOP_CANCELLED == downtime_data->attr)
        params.cancelled = true;
      params.end_time = downtime_data->timestamp.tv_sec;
    } else if (NEBTYPE_DOWNTIME_DELETE == downtime_data->type) {
      if (!params.started)
        params.cancelled = true;
      params.deletion_time = downtime_data->timestamp.tv_sec;
    }
    downtime->actual_start_time = params.start_time;
    downtime->actual_end_time = params.end_time;
    downtime->deletion_time = params.deletion_time;
    downtime->was_cancelled = params.cancelled;
    downtime->was_started = params.started;
    if (NEBTYPE_DOWNTIME_DELETE == downtime_data->type)
      downtimes.erase(downtime->internal_id);

    // Send event.
    gl_publisher.write(downtime);
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       "generating downtime event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process event handler data.
 *
 *  This function is called by Nagios when some event handler data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_EVENT_HANDLER_DATA).
 *  @param[in] data          A pointer to a nebstruct_event_handler_data
 *                           containing the event handler data.
 *
 *  @return 0 on success.
 */
int neb::callback_event_handler(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating event handler event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_event_handler_data const* event_handler_data;
    std::shared_ptr<neb::event_handler> event_handler(new neb::event_handler);

    // Fill output var.
    event_handler_data = static_cast<nebstruct_event_handler_data*>(data);
    if (event_handler_data->command_args)
      event_handler->command_args =
          misc::string::check_string_utf8(event_handler_data->command_args);
    if (event_handler_data->command_line)
      event_handler->command_line =
          misc::string::check_string_utf8(event_handler_data->command_line);
    event_handler->early_timeout = event_handler_data->early_timeout;
    event_handler->end_time = event_handler_data->end_time.tv_sec;
    event_handler->execution_time = event_handler_data->execution_time;
    if (!event_handler_data->host_name)
      throw(exceptions::msg() << "unnamed host");
    if (event_handler_data->service_description) {
      std::pair<uint32_t, uint32_t> p;
      p = engine::get_host_and_service_id(
          event_handler_data->host_name,
          event_handler_data->service_description);
      event_handler->host_id = p.first;
      event_handler->service_id = p.second;
      if (!event_handler->host_id || !event_handler->service_id)
        throw(exceptions::msg()
              << "could not find ID of service ('"
              << event_handler_data->host_name << "', '"
              << event_handler_data->service_description << "')");
    } else {
      event_handler->host_id =
          engine::get_host_id(event_handler_data->host_name);
      if (event_handler->host_id == 0)
        throw(exceptions::msg() << "could not find ID of host '"
                                << event_handler_data->host_name << "'");
    }
    if (event_handler_data->output)
      event_handler->output =
          misc::string::check_string_utf8(event_handler_data->output);
    event_handler->return_code = event_handler_data->return_code;
    event_handler->start_time = event_handler_data->start_time.tv_sec;
    event_handler->state = event_handler_data->state;
    event_handler->state_type = event_handler_data->state_type;
    event_handler->timeout = event_handler_data->timeout;
    event_handler->handler_type = event_handler_data->eventhandler_type;

    // Send event.
    gl_publisher.write(event_handler);
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating event handler event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process external commands.
 *
 *  This function is called by the monitoring engine when some external
 *  command is received.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_EXTERNALCOMMAND_DATA).
 *  @param[in] data          A pointer to a nebstruct_externalcommand_data
 *                           containing the external command data.
 *
 *  @return 0 on success.
 */
int neb::callback_external_command(int callback_type, void* data) {
  // Log message.
  logging::debug(logging::low) << "callbacks: external command data";
  (void)callback_type;

  nebstruct_external_command_data* necd(
      static_cast<nebstruct_external_command_data*>(data));
  if (necd && (necd->type == NEBTYPE_EXTERNALCOMMAND_START)) {
    try {
      if (necd->command_type == CMD_CHANGE_CUSTOM_HOST_VAR) {
        logging::info(logging::medium)
            << "callbacks: generating host custom variable update event";

        // Split argument string.
        if (necd->command_args) {
          std::list<std::string> l{misc::string::split(
              misc::string::check_string_utf8(necd->command_args), ';')};
          if (l.size() != 3)
            logging::error(logging::medium)
                << "callbacks: invalid host custom variable command";
          else {
            std::list<std::string>::iterator it(l.begin());
            std::string host{*it++};
            std::string var_name{*it++};
            std::string var_value{*it};

            // Find host ID.
            uint64_t host_id = engine::get_host_id(host);
            if (host_id != 0) {
              // Fill custom variable.
              std::shared_ptr<neb::custom_variable_status> cvs{
                  new neb::custom_variable_status};
              cvs->host_id = host_id;
              cvs->modified = true;
              cvs->name = var_name;
              cvs->service_id = 0;
              cvs->update_time = necd->timestamp.tv_sec;
              cvs->value = var_value;

              // Send event.
              gl_publisher.write(cvs);
            }
          }
        }
      } else if (necd->command_type == CMD_CHANGE_CUSTOM_SVC_VAR) {
        logging::info(logging::medium)
            << "callbacks: generating service custom variable update event";

        // Split argument string.
        if (necd->command_args) {
          std::list<std::string> l{misc::string::split(
              misc::string::check_string_utf8(necd->command_args), ';')};
          if (l.size() != 4)
            logging::error(logging::medium)
                << "callbacks: invalid service custom variable command";
          else {
            std::list<std::string>::iterator it{l.begin()};
            std::string host{*it++};
            std::string service{*it++};
            std::string var_name{*it++};
            std::string var_value{*it};

            // Find host/service IDs.
            std::pair<uint64_t, uint64_t> p{
                engine::get_host_and_service_id(host, service)};
            if (p.first && p.second) {
              // Fill custom variable.
              std::shared_ptr<neb::custom_variable_status> cvs{
                  new neb::custom_variable_status};
              cvs->host_id = p.first;
              cvs->modified = true;
              cvs->name = var_name;
              cvs->service_id = p.second;
              cvs->update_time = necd->timestamp.tv_sec;
              cvs->value = var_value;

              // Send event.
              gl_publisher.write(cvs);
            }
          }
        }
      }
    }
    // Avoid exception propagation in C code.
    catch (...) {
    }
  }
  return 0;
}

/**
 *  @brief Function that process flapping status data.
 *
 *  This function is called by Nagios when some flapping status data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_FLAPPING_DATA).
 *  @param[in] data          A pointer to a nebstruct_flapping_data
 *                           containing the flapping status data.
 *
 *  @return 0 on success.
 */
int neb::callback_flapping_status(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating flapping event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_flapping_data const* flapping_data;
    std::shared_ptr<neb::flapping_status> flapping_status(
        new neb::flapping_status);

    // Fill output var.
    flapping_data = static_cast<nebstruct_flapping_data*>(data);
    flapping_status->event_time = flapping_data->timestamp.tv_sec;
    flapping_status->event_type = flapping_data->type;
    flapping_status->high_threshold = flapping_data->high_threshold;
    if (flapping_data->service_id == 0) {
      flapping_status->host_id = flapping_data->host_id;
    } else {
      flapping_status->host_id = flapping_data->host_id;
      flapping_status->service_id = flapping_data->service_id;
    }
    flapping_status->low_threshold = flapping_data->low_threshold;
    flapping_status->percent_state_change = flapping_data->percent_change;
    // flapping_status->reason_type = XXX;
    flapping_status->flapping_type = flapping_data->flapping_type;

    // Send event.
    gl_publisher.write(flapping_status);
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       "generating flapping event: "
                                    << e.what();
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process group data.
 *
 *  This function is called by Engine when some group data is available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_GROUP_DATA).
 *  @param[in] data          Pointer to a nebstruct_group_data
 *                           containing the group data.
 *
 *  @return 0 on success.
 */
int neb::callback_group(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating group event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_group_data const* group_data(
        static_cast<nebstruct_group_data*>(data));

    // Host group.
    if ((NEBTYPE_HOSTGROUP_ADD == group_data->type) ||
        (NEBTYPE_HOSTGROUP_UPDATE == group_data->type) ||
        (NEBTYPE_HOSTGROUP_DELETE == group_data->type)) {
      engine::hostgroup const* host_group(
          static_cast<engine::hostgroup*>(group_data->object_ptr));
      if (!host_group->get_group_name().empty()) {
        std::shared_ptr<neb::host_group> new_hg(new neb::host_group);
        new_hg->poller_id = config::applier::state::instance().poller_id();
        new_hg->id = host_group->get_id();
        new_hg->enabled = (group_data->type != NEBTYPE_HOSTGROUP_DELETE &&
                           !host_group->members.empty());
        new_hg->name =
            misc::string::check_string_utf8(host_group->get_group_name());

        // Send host group event.
        if (new_hg->id) {
          logging::info(logging::low)
              << "callbacks: new host group " << new_hg->id << " ('"
              << new_hg->name << "') on instance " << new_hg->poller_id;
          neb::gl_publisher.write(new_hg);
        }
      }
    }
    // Service group.
    else if ((NEBTYPE_SERVICEGROUP_ADD == group_data->type) ||
             (NEBTYPE_SERVICEGROUP_UPDATE == group_data->type) ||
             (NEBTYPE_SERVICEGROUP_DELETE == group_data->type)) {
      engine::servicegroup const* service_group(
          static_cast<engine::servicegroup*>(group_data->object_ptr));
      if (!service_group->get_group_name().empty()) {
        std::shared_ptr<neb::service_group> new_sg(new neb::service_group);
        new_sg->poller_id = config::applier::state::instance().poller_id();
        new_sg->id = service_group->get_id();
        new_sg->enabled = (group_data->type != NEBTYPE_SERVICEGROUP_DELETE &&
                           !service_group->members.empty());
        new_sg->name =
            misc::string::check_string_utf8(service_group->get_group_name());

        // Send service group event.
        if (new_sg->id) {
          logging::info(logging::low)
              << "callbacks:: new service group " << new_sg->id << " ('"
              << new_sg->name << "') on instance " << new_sg->poller_id;
          neb::gl_publisher.write(new_sg);
        }
      }
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process group membership.
 *
 *  This function is called by Engine when some group membership data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_GROUPMEMBER_DATA).
 *  @param[in] data          Pointer to a nebstruct_group_member_data
 *                           containing membership data.
 *
 *  @return 0 on success.
 */
int neb::callback_group_member(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating group member event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_group_member_data const* member_data(
        static_cast<nebstruct_group_member_data*>(data));

    // Host group member.
    if ((member_data->type == NEBTYPE_HOSTGROUPMEMBER_ADD) ||
        (member_data->type == NEBTYPE_HOSTGROUPMEMBER_DELETE)) {
      engine::host const* hst(
          static_cast<engine::host*>(member_data->object_ptr));
      engine::hostgroup const* hg(
          static_cast<engine::hostgroup*>(member_data->group_ptr));
      if (!hst->get_name().empty() && !hg->get_group_name().empty()) {
        // Output variable.
        std::shared_ptr<neb::host_group_member> hgm(new neb::host_group_member);
        hgm->group_id = hg->get_id();
        hgm->group_name = misc::string::check_string_utf8(hg->get_group_name());
        hgm->poller_id = config::applier::state::instance().poller_id();
        uint32_t host_id = engine::get_host_id(hst->get_name());
        if (host_id != 0 && hgm->group_id != 0) {
          hgm->host_id = host_id;
          if (member_data->type == NEBTYPE_HOSTGROUPMEMBER_DELETE) {
            logging::info(logging::low)
                << "callbacks: host " << hgm->host_id
                << " is not a member of group " << hgm->group_id
                << " on instance " << hgm->poller_id << " anymore";
            hgm->enabled = false;
          } else {
            logging::info(logging::low)
                << "callbacks: host " << hgm->host_id
                << " is a member of group " << hgm->group_id << " on instance "
                << hgm->poller_id;
            hgm->enabled = true;
          }

          // Send host group member event.
          if (hgm->host_id && hgm->group_id)
            neb::gl_publisher.write(hgm);
        }
      }
    }
    // Service group member.
    else if ((member_data->type == NEBTYPE_SERVICEGROUPMEMBER_ADD) ||
             (member_data->type == NEBTYPE_SERVICEGROUPMEMBER_DELETE)) {
      engine::service const* svc(
          static_cast<engine::service*>(member_data->object_ptr));
      engine::servicegroup const* sg(
          static_cast<engine::servicegroup*>(member_data->group_ptr));
      if (!svc->get_description().empty() && !sg->get_group_name().empty() &&
          !svc->get_hostname().empty()) {
        // Output variable.
        std::shared_ptr<neb::service_group_member> sgm(
            new neb::service_group_member);
        sgm->group_id = sg->get_id();
        sgm->group_name = misc::string::check_string_utf8(sg->get_group_name());
        sgm->poller_id = config::applier::state::instance().poller_id();
        std::pair<uint32_t, uint32_t> p;
        p = engine::get_host_and_service_id(svc->get_hostname(),
                                            svc->get_description());
        sgm->host_id = p.first;
        sgm->service_id = p.second;
        if (sgm->host_id && sgm->service_id && sgm->group_id) {
          if (member_data->type == NEBTYPE_SERVICEGROUPMEMBER_DELETE) {
            logging::info(logging::low)
                << "callbacks: service (" << sgm->host_id << ", "
                << sgm->service_id << ") is not a member of group "
                << sgm->group_id << " on instance " << sgm->poller_id
                << " anymore";
            sgm->enabled = false;
          } else {
            logging::info(logging::low)
                << "callbacks: service (" << sgm->host_id << ", "
                << sgm->service_id << ") is a member of group " << sgm->group_id
                << " on instance " << sgm->poller_id;
            sgm->enabled = true;
          }

          // Send service group member event.
          if (sgm->host_id && sgm->service_id && sgm->group_id)
            neb::gl_publisher.write(sgm);
        }
      }
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process host data.
 *
 *  This function is called by Engine when some host data is available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_DATA).
 *  @param[in] data          A pointer to a nebstruct_host_data
 *                           containing a host data.
 *
 *  @return 0 on success.
 */
int neb::callback_host(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating host event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_adaptive_host_data const* host_data(
        static_cast<nebstruct_adaptive_host_data*>(data));
    engine::host const* h(static_cast<engine::host*>(host_data->object_ptr));
    std::shared_ptr<neb::host> my_host(new neb::host);

    // Set host parameters.
    my_host->acknowledged = h->get_problem_has_been_acknowledged();
    my_host->acknowledgement_type = h->get_acknowledgement_type();
    if (!h->get_action_url().empty())
      my_host->action_url =
          misc::string::check_string_utf8(h->get_action_url());
    my_host->active_checks_enabled = h->get_checks_enabled();
    if (!h->get_address().empty())
      my_host->address = misc::string::check_string_utf8(h->get_address());
    if (!h->get_alias().empty())
      my_host->alias = misc::string::check_string_utf8(h->get_alias());
    my_host->check_freshness = h->get_check_freshness();
    if (!h->get_check_command().empty())
      my_host->check_command =
          misc::string::check_string_utf8(h->get_check_command());
    my_host->check_interval = h->get_check_interval();
    if (!h->get_check_period().empty())
      my_host->check_period = h->get_check_period();
    my_host->check_type = h->get_check_type();
    my_host->current_check_attempt = h->get_current_attempt();
    my_host->current_state = (h->get_has_been_checked() ? h->get_current_state()
                                                        : 4);  // Pending state.
    my_host->default_active_checks_enabled = h->get_checks_enabled();
    my_host->default_event_handler_enabled = h->get_event_handler_enabled();
    my_host->default_flap_detection_enabled = h->get_flap_detection_enabled();
    my_host->default_notifications_enabled = h->get_notifications_enabled();
    my_host->default_passive_checks_enabled = h->get_accept_passive_checks();
    my_host->downtime_depth = h->get_scheduled_downtime_depth();
    if (!h->get_display_name().empty())
      my_host->display_name =
          misc::string::check_string_utf8(h->get_display_name());
    my_host->enabled = (host_data->type != NEBTYPE_HOST_DELETE);
    if (!h->get_event_handler().empty())
      my_host->event_handler =
          misc::string::check_string_utf8(h->get_event_handler());
    my_host->event_handler_enabled = h->get_event_handler_enabled();
    my_host->execution_time = h->get_execution_time();
    my_host->first_notification_delay = h->get_first_notification_delay();
    my_host->notification_number = h->get_notification_number();
    my_host->flap_detection_enabled = h->get_flap_detection_enabled();
    my_host->flap_detection_on_down =
        h->get_flap_detection_on(engine::notifier::down);
    my_host->flap_detection_on_unreachable =
        h->get_flap_detection_on(engine::notifier::unreachable);
    my_host->flap_detection_on_up =
        h->get_flap_detection_on(engine::notifier::up);
    my_host->freshness_threshold = h->get_freshness_threshold();
    my_host->has_been_checked = h->get_has_been_checked();
    my_host->high_flap_threshold = h->get_high_flap_threshold();
    if (!h->get_name().empty())
      my_host->host_name = misc::string::check_string_utf8(h->get_name());
    if (!h->get_icon_image().empty())
      my_host->icon_image =
          misc::string::check_string_utf8(h->get_icon_image());
    if (!h->get_icon_image_alt().empty())
      my_host->icon_image_alt =
          misc::string::check_string_utf8(h->get_icon_image_alt());
    my_host->is_flapping = h->get_is_flapping();
    my_host->last_check = h->get_last_check();
    my_host->last_hard_state = h->get_last_hard_state();
    my_host->last_hard_state_change = h->get_last_hard_state_change();
    my_host->last_notification = h->get_last_notification();
    my_host->last_state_change = h->get_last_state_change();
    my_host->last_time_down = h->get_last_time_down();
    my_host->last_time_unreachable = h->get_last_time_unreachable();
    my_host->last_time_up = h->get_last_time_up();
    my_host->last_update = time(nullptr);
    my_host->latency = h->get_latency();
    my_host->low_flap_threshold = h->get_low_flap_threshold();
    my_host->max_check_attempts = h->get_max_attempts();
    my_host->next_check = h->get_next_check();
    my_host->next_notification = h->get_next_notification();
    my_host->no_more_notifications = h->get_no_more_notifications();
    if (!h->get_notes().empty())
      my_host->notes = misc::string::check_string_utf8(h->get_notes());
    if (!h->get_notes_url().empty())
      my_host->notes_url = misc::string::check_string_utf8(h->get_notes_url());
    my_host->notifications_enabled = h->get_notifications_enabled();
    my_host->notification_interval = h->get_notification_interval();
    if (!h->get_notification_period().empty())
      my_host->notification_period = h->get_notification_period();
    my_host->notify_on_down = h->get_notify_on(engine::notifier::down);
    my_host->notify_on_downtime = h->get_notify_on(engine::notifier::downtime);
    my_host->notify_on_flapping =
        h->get_notify_on(engine::notifier::flappingstart);
    my_host->notify_on_recovery = h->get_notify_on(engine::notifier::up);
    my_host->notify_on_unreachable =
        h->get_notify_on(engine::notifier::unreachable);
    my_host->obsess_over = h->get_obsess_over();
    if (!h->get_plugin_output().empty()) {
      my_host->output = misc::string::check_string_utf8(h->get_plugin_output());
      my_host->output.append("\n");
    }
    if (!h->get_long_plugin_output().empty())
      my_host->output.append(
          misc::string::check_string_utf8(h->get_long_plugin_output()));
    my_host->passive_checks_enabled = h->get_accept_passive_checks();
    my_host->percent_state_change = h->get_percent_state_change();
    if (!h->get_perf_data().empty())
      my_host->perf_data = misc::string::check_string_utf8(h->get_perf_data());
    my_host->poller_id = config::applier::state::instance().poller_id();
    my_host->retain_nonstatus_information =
        h->get_retain_nonstatus_information();
    my_host->retain_status_information = h->get_retain_status_information();
    my_host->retry_interval = h->get_retry_interval();
    my_host->should_be_scheduled = h->get_should_be_scheduled();
    my_host->stalk_on_down = h->get_stalk_on(engine::notifier::down);
    my_host->stalk_on_unreachable =
        h->get_stalk_on(engine::notifier::unreachable);
    my_host->stalk_on_up = h->get_stalk_on(engine::notifier::up);
    my_host->state_type = (h->get_has_been_checked() ? h->get_state_type()
                                                     : engine::notifier::hard);
    if (!h->get_statusmap_image().empty())
      my_host->statusmap_image =
          misc::string::check_string_utf8(h->get_statusmap_image());
    my_host->timezone = h->get_timezone();

    // Find host ID.
    uint64_t host_id = engine::get_host_id(my_host->host_name);
    if (host_id != 0) {
      my_host->host_id = host_id;

      // Send host event.
      logging::info(logging::low)
          << "callbacks:  new host " << my_host->host_id << " ('"
          << my_host->host_name << "') on instance " << my_host->poller_id;
      neb::gl_publisher.write(my_host);

      /* No need to send this service custom variables changes, custom variables
       * are managed in a different loop. */
    } else
      logging::error(logging::medium)
          << "callbacks: host '"
          << (!h->get_name().empty() ? h->get_name() : "(unknown)")
          << "' has no ID (yet) defined";
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process host check data.
 *
 *  This function is called by Nagios when some host check data are available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_CHECK_DATA).
 *  @param[in] data          A pointer to a nebstruct_host_check_data
 *                           containing the host check data.
 *
 *  @return 0 on success.
 */
int neb::callback_host_check(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating host check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_host_check_data const* hcdata;
    std::shared_ptr<neb::host_check> host_check(new neb::host_check);

    // Fill output var.
    hcdata = static_cast<nebstruct_host_check_data*>(data);
    engine::host* h(static_cast<engine::host*>(hcdata->object_ptr));
    if (hcdata->command_line) {
      host_check->active_checks_enabled = h->get_checks_enabled();
      host_check->check_type = hcdata->check_type;
      host_check->command_line =
          misc::string::check_string_utf8(hcdata->command_line);
      if (!hcdata->host_name)
        throw(exceptions::msg() << "unnamed host");
      host_check->host_id = engine::get_host_id(hcdata->host_name);
      if (host_check->host_id == 0)
        throw(exceptions::msg()
              << "could not find ID of host '" << hcdata->host_name << "'");
      host_check->next_check = h->get_next_check();

      // Send event.
      gl_publisher.write(host_check);
    }
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating host check event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process host status data.
 *
 *  This function is called by Nagios when some host status data are available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_HOST_STATUS_DATA).
 *  @param[in] data          A pointer to a nebstruct_host_status_data
 *                           containing the host status data.
 *
 *  @return 0 on success.
 */
int neb::callback_host_status(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating host status event";
  (void)callback_type;

  try {
    // In/Out variables.
    engine::host const* h;
    std::shared_ptr<neb::host_status> host_status(new neb::host_status);

    // Fill output var.
    h = static_cast<engine::host*>(
        static_cast<nebstruct_host_status_data*>(data)->object_ptr);
    host_status->acknowledged = h->get_problem_has_been_acknowledged();
    host_status->acknowledgement_type = h->get_acknowledgement_type();
    host_status->active_checks_enabled = h->get_checks_enabled();
    if (!h->get_check_command().empty())
      host_status->check_command =
          misc::string::check_string_utf8(h->get_check_command());
    host_status->check_interval = h->get_check_interval();
    if (!h->get_check_period().empty())
      host_status->check_period = h->get_check_period();
    host_status->check_type = h->get_check_type();
    host_status->current_check_attempt = h->get_current_attempt();
    host_status->current_state =
        (h->get_has_been_checked() ? h->get_current_state()
                                   : 4);  // Pending state.
    host_status->downtime_depth = h->get_scheduled_downtime_depth();
    if (!h->get_event_handler().empty())
      host_status->event_handler =
          misc::string::check_string_utf8(h->get_event_handler());
    host_status->event_handler_enabled = h->get_event_handler_enabled();
    host_status->execution_time = h->get_execution_time();
    host_status->flap_detection_enabled = h->get_flap_detection_enabled();
    host_status->has_been_checked = h->get_has_been_checked();
    if (h->get_name().empty())
      throw(exceptions::msg() << "unnamed host");
    {
      host_status->host_id = engine::get_host_id(h->get_name());
      if (host_status->host_id == 0)
        throw(exceptions::msg()
              << "could not find ID of host '" << h->get_name() << "'");
    }
    host_status->is_flapping = h->get_is_flapping();
    host_status->last_check = h->get_last_check();
    host_status->last_hard_state = h->get_last_hard_state();
    host_status->last_hard_state_change = h->get_last_hard_state_change();
    host_status->last_notification = h->get_last_notification();
    host_status->notification_number = h->get_notification_number();
    host_status->last_state_change = h->get_last_state_change();
    host_status->last_time_down = h->get_last_time_down();
    host_status->last_time_unreachable = h->get_last_time_unreachable();
    host_status->last_time_up = h->get_last_time_up();
    host_status->last_update = time(nullptr);
    host_status->latency = h->get_latency();
    host_status->max_check_attempts = h->get_max_attempts();
    host_status->next_check = h->get_next_check();
    host_status->next_notification = h->get_next_notification();
    host_status->no_more_notifications = h->get_no_more_notifications();
    host_status->notifications_enabled = h->get_notifications_enabled();
    host_status->obsess_over = h->get_obsess_over();
    if (!h->get_plugin_output().empty()) {
      host_status->output =
          misc::string::check_string_utf8(h->get_plugin_output());
      host_status->output.append("\n");
    }
    if (!h->get_long_plugin_output().empty())
      host_status->output.append(
          misc::string::check_string_utf8(h->get_long_plugin_output()));
    host_status->passive_checks_enabled = h->get_accept_passive_checks();
    host_status->percent_state_change = h->get_percent_state_change();
    if (!h->get_perf_data().empty())
      host_status->perf_data =
          misc::string::check_string_utf8(h->get_perf_data());
    host_status->retry_interval = h->get_retry_interval();
    host_status->should_be_scheduled = h->get_should_be_scheduled();
    host_status->state_type =
        (h->get_has_been_checked() ? h->get_state_type()
                                   : engine::notifier::hard);

    // Send event(s).
    gl_publisher.write(host_status);

    // Acknowledgement event.
    std::map<std::pair<uint32_t, uint32_t>, neb::acknowledgement>::iterator it(
        gl_acknowledgements.find(std::make_pair(host_status->host_id, 0u)));
    if ((it != gl_acknowledgements.end()) && !host_status->acknowledged) {
      if (!(!host_status->current_state  // !(OK or (normal ack and NOK))
            || (!it->second.is_sticky &&
                (host_status->current_state != it->second.state)))) {
        std::shared_ptr<neb::acknowledgement> ack(
            new neb::acknowledgement(it->second));
        ack->deletion_time = time(nullptr);
        gl_publisher.write(ack);
      }
      gl_acknowledgements.erase(it);
    }
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating host status event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process log data.
 *
 *  This function is called by Nagios when some log data are available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_LOG_DATA).
 *  @param[in] data          A pointer to a nebstruct_log_data containing the
 *                           log data.
 *
 *  @return 0 on success.
 */
int neb::callback_log(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating log event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_log_data const* log_data;
    std::shared_ptr<neb::log_entry> le(new neb::log_entry);

    // Fill output var.
    log_data = static_cast<nebstruct_log_data*>(data);
    le->c_time = log_data->entry_time;
    le->poller_name = config::applier::state::instance().poller_name();
    if (log_data->data) {
      le->output = misc::string::check_string_utf8(log_data->data);
      set_log_data(*le, le->output.c_str());
    }

    // Send event.
    gl_publisher.write(le);
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process module data.
 *
 *  This function is called by Engine when some module data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_MODULE_DATA).
 *  @param[in] data          A pointer to a nebstruct_module_data
 *                           containing the module data.
 *
 *  @return 0 on success.
 */
int neb::callback_module(int callback_type, void* data) {
  // Log message.
  logging::debug(logging::low) << "callbacks: generating module event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_module_data const* module_data;
    std::shared_ptr<neb::module> me(new neb::module);

    // Fill output var.
    module_data = static_cast<nebstruct_module_data*>(data);
    if (module_data->module) {
      me->poller_id = config::applier::state::instance().poller_id();
      me->filename = misc::string::check_string_utf8(module_data->module);
      if (module_data->args)
        me->args = misc::string::check_string_utf8(module_data->args);
      me->loaded = !(module_data->type == NEBTYPE_MODULE_DELETE);
      me->should_be_loaded = true;

      // Send events.
      gl_publisher.write(me);
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process process data.
 *
 *  This function is called by Nagios when some process data is available.
 *
 *  @param[in] callback_type Type of the callback (NEBCALLBACK_PROCESS_DATA).
 *  @param[in] data          A pointer to a nebstruct_process_data containing
 *                           the process data.
 *
 *  @return 0 on success.
 */
int neb::callback_process(int callback_type, void* data) {
  // Log message.
  logging::debug(logging::low) << "callbacks: process event callback";
  (void)callback_type;

  try {
    // Input variables.
    nebstruct_process_data const* process_data;
    static time_t start_time;

    // Check process event type.
    process_data = static_cast<nebstruct_process_data*>(data);
    if (NEBTYPE_PROCESS_EVENTLOOPSTART == process_data->type) {
      logging::info(logging::medium)
          << "callbacks: generating process start event";

      // Register callbacks.
      logging::debug(logging::high) << "callbacks: registering callbacks";
      for (uint32_t i(0); i < sizeof(gl_callbacks) / sizeof(*gl_callbacks); ++i)
        gl_registered_callbacks.push_back(std::shared_ptr<callback>(
            new neb::callback(gl_callbacks[i].macro, gl_mod_handle,
                              gl_callbacks[i].callback)));

      // Register Engine-specific callbacks.
      if (gl_mod_flags & NEBMODULE_ENGINE) {
        for (uint32_t i(0);
             i < sizeof(gl_engine_callbacks) / sizeof(*gl_engine_callbacks);
             ++i)
          gl_registered_callbacks.push_back(std::shared_ptr<callback>(
              new neb::callback(gl_engine_callbacks[i].macro, gl_mod_handle,
                                gl_engine_callbacks[i].callback)));
      }

      // Parse configuration file.
      uint32_t statistics_interval(0);
      try {
        config::parser parsr;
        config::state conf{parsr.parse(gl_configuration_file)};

        // Apply resulting configuration.
        config::applier::state::instance().apply(conf);
        gl_generator.set(conf);

        // Set variables.
        statistics_interval = gl_generator.interval();
      } catch (exceptions::msg const& e) {
        logging::config(logging::high) << e.what();
        return 0;
      }

      // Output variable.
      std::shared_ptr<neb::instance> instance(new neb::instance);
      instance->poller_id = config::applier::state::instance().poller_id();
      instance->engine = "Centreon Engine";
      instance->is_running = true;
      instance->name = config::applier::state::instance().poller_name();
      instance->pid = getpid();
      instance->program_start = time(nullptr);
      instance->version = get_program_version();
      start_time = instance->program_start;

      // Send initial event and then configuration.
      gl_publisher.write(instance);
      send_initial_configuration();

      // Add statistics event.
      if (statistics_interval) {
        logging::info(logging::medium)
            << "stats: registering statistics generation event in "
            << "monitoring engine";
        union {
          void (*code)(void*);
          void* data;
        } val;
        val.code = &event_statistics;
        com::centreon::engine::timed_event* evt =
            new com::centreon::engine::timed_event(
                com::centreon::engine::timed_event::EVENT_USER_FUNCTION,
                time(nullptr) + statistics_interval, 1, statistics_interval,
                nullptr, 1, val.data, nullptr, 0);
        com::centreon::engine::events::loop::instance().schedule(evt, false);
      }
    } else if (NEBTYPE_PROCESS_EVENTLOOPEND == process_data->type) {
      logging::info(logging::medium)
          << "callbacks: generating process end event";
      // Output variable.
      std::shared_ptr<neb::instance> instance(new neb::instance);

      // Fill output var.
      instance->poller_id = config::applier::state::instance().poller_id();
      instance->engine = "Centreon Engine";
      instance->is_running = false;
      instance->name = config::applier::state::instance().poller_name();
      instance->pid = getpid();
      instance->program_end = time(nullptr);
      instance->program_start = start_time;
      instance->version = get_program_version();

      // Send event.
      gl_publisher.write(instance);
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {
    unregister_callbacks();
  }
  return 0;
}

/**
 *  @brief Function that process instance status data.
 *
 *  This function is called by Nagios when some instance status data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_PROGRAM_STATUS_DATA).
 *  @param[in] data          A pointer to a nebstruct_program_status_data
 *                           containing the program status data.
 *
 *  @return 0 on success.
 */
int neb::callback_program_status(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium)
      << "callbacks: generating instance status event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_program_status_data const* program_status_data;
    std::shared_ptr<neb::instance_status> is(new neb::instance_status);

    // Fill output var.
    program_status_data = static_cast<nebstruct_program_status_data*>(data);
    is->poller_id = config::applier::state::instance().poller_id();
    is->active_host_checks_enabled =
        program_status_data->active_host_checks_enabled;
    is->active_service_checks_enabled =
        program_status_data->active_service_checks_enabled;
    is->check_hosts_freshness = check_host_freshness;
    is->check_services_freshness = check_service_freshness;
    is->event_handler_enabled = program_status_data->event_handlers_enabled;
    is->flap_detection_enabled = program_status_data->flap_detection_enabled;
    if (program_status_data->global_host_event_handler)
      is->global_host_event_handler = misc::string::check_string_utf8(
          program_status_data->global_host_event_handler);
    if (program_status_data->global_service_event_handler)
      is->global_service_event_handler = misc::string::check_string_utf8(
          program_status_data->global_service_event_handler);
    is->last_alive = time(nullptr);
    is->last_command_check = program_status_data->last_command_check;
    is->notifications_enabled = program_status_data->notifications_enabled;
    is->obsess_over_hosts = program_status_data->obsess_over_hosts;
    is->obsess_over_services = program_status_data->obsess_over_services;
    is->passive_host_checks_enabled =
        program_status_data->passive_host_checks_enabled;
    is->passive_service_checks_enabled =
        program_status_data->passive_service_checks_enabled;

    // Send event.
    gl_publisher.write(is);
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process relation data.
 *
 *  This function is called by Engine when some relation data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_RELATION_DATA).
 *  @param[in] data          Pointer to a nebstruct_relation_data
 *                           containing the relationship.
 *
 *  @return 0 on success.
 */
int neb::callback_relation(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating relation event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_relation_data const* relation(
        static_cast<nebstruct_relation_data*>(data));

    // Host parent.
    if ((NEBTYPE_PARENT_ADD == relation->type) ||
        (NEBTYPE_PARENT_DELETE == relation->type)) {
      if (relation->hst && relation->dep_hst && !relation->svc &&
          !relation->dep_svc) {
        // Find host IDs.
        int host_id;
        int parent_id;
        {
          host_id = engine::get_host_id(relation->dep_hst->get_name());
          parent_id = engine::get_host_id(relation->hst->get_name());
        }
        if (host_id && parent_id) {
          // Generate parent event.
          std::shared_ptr<host_parent> new_host_parent(new host_parent);
          new_host_parent->enabled = (relation->type != NEBTYPE_PARENT_DELETE);
          new_host_parent->host_id = host_id;
          new_host_parent->parent_id = parent_id;

          // Send event.
          logging::info(logging::low)
              << "callbacks: host " << new_host_parent->parent_id
              << " is parent of host " << new_host_parent->host_id;
          neb::gl_publisher.write(new_host_parent);
        }
      }
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process service data.
 *
 *  This function is called by Engine when some service data is
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_ADAPTIVE_SERVICE_DATA).
 *  @param[in] data          A pointer to a
 *                           nebstruct_adaptive_service_data containing
 *                           the service data.
 *
 *  @return 0 on success.
 */
int neb::callback_service(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating service event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_adaptive_service_data const* service_data(
        static_cast<nebstruct_adaptive_service_data*>(data));
    engine::service const* s(
        static_cast<engine::service*>(service_data->object_ptr));
    std::shared_ptr<neb::service> my_service(new neb::service);

    // Fill output var.
    my_service->acknowledged = s->get_problem_has_been_acknowledged();
    my_service->acknowledgement_type = s->get_acknowledgement_type();
    if (!s->get_action_url().empty())
      my_service->action_url =
          misc::string::check_string_utf8(s->get_action_url());
    my_service->active_checks_enabled = s->get_checks_enabled();
    if (!s->get_check_command().empty())
      my_service->check_command =
          misc::string::check_string_utf8(s->get_check_command());
    my_service->check_freshness = s->get_check_freshness();
    my_service->check_interval = s->get_check_interval();
    if (!s->get_check_period().empty())
      my_service->check_period = s->get_check_period();
    my_service->check_type = s->get_check_type();
    my_service->current_check_attempt = s->get_current_attempt();
    my_service->current_state =
        (s->get_has_been_checked() ? s->get_current_state()
                                   : 4);  // Pending state.
    my_service->default_active_checks_enabled = s->get_checks_enabled();
    my_service->default_event_handler_enabled = s->get_event_handler_enabled();
    my_service->default_flap_detection_enabled =
        s->get_flap_detection_enabled();
    my_service->default_notifications_enabled = s->get_notifications_enabled();
    my_service->default_passive_checks_enabled = s->get_accept_passive_checks();
    my_service->downtime_depth = s->get_scheduled_downtime_depth();
    if (!s->get_display_name().empty())
      my_service->display_name =
          misc::string::check_string_utf8(s->get_display_name());
    my_service->enabled = (service_data->type != NEBTYPE_SERVICE_DELETE);
    if (!s->get_event_handler().empty())
      my_service->event_handler =
          misc::string::check_string_utf8(s->get_event_handler());
    my_service->event_handler_enabled = s->get_event_handler_enabled();
    my_service->execution_time = s->get_execution_time();
    my_service->first_notification_delay = s->get_first_notification_delay();
    my_service->notification_number = s->get_notification_number();
    my_service->flap_detection_enabled = s->get_flap_detection_enabled();
    my_service->flap_detection_on_critical =
        s->get_flap_detection_on(engine::notifier::critical);
    my_service->flap_detection_on_ok =
        s->get_flap_detection_on(engine::notifier::ok);
    my_service->flap_detection_on_unknown =
        s->get_flap_detection_on(engine::notifier::unknown);
    my_service->flap_detection_on_warning =
        s->get_flap_detection_on(engine::notifier::warning);
    my_service->freshness_threshold = s->get_freshness_threshold();
    my_service->has_been_checked = s->get_has_been_checked();
    my_service->high_flap_threshold = s->get_high_flap_threshold();
    if (!s->get_hostname().empty())
      my_service->host_name =
          misc::string::check_string_utf8(s->get_hostname());
    if (!s->get_icon_image().empty())
      my_service->icon_image =
          misc::string::check_string_utf8(s->get_icon_image());
    if (!s->get_icon_image_alt().empty())
      my_service->icon_image_alt =
          misc::string::check_string_utf8(s->get_icon_image_alt());
    my_service->is_flapping = s->get_is_flapping();
    my_service->is_volatile = s->get_is_volatile();
    my_service->last_check = s->get_last_check();
    my_service->last_hard_state = s->get_last_hard_state();
    my_service->last_hard_state_change = s->get_last_hard_state_change();
    my_service->last_notification = s->get_last_notification();
    my_service->last_state_change = s->get_last_state_change();
    my_service->last_time_critical = s->get_last_time_critical();
    my_service->last_time_ok = s->get_last_time_ok();
    my_service->last_time_unknown = s->get_last_time_unknown();
    my_service->last_time_warning = s->get_last_time_warning();
    my_service->last_update = time(nullptr);
    my_service->latency = s->get_latency();
    my_service->low_flap_threshold = s->get_low_flap_threshold();
    my_service->max_check_attempts = s->get_max_attempts();
    my_service->next_check = s->get_next_check();
    my_service->next_notification = s->get_next_notification();
    my_service->no_more_notifications = s->get_no_more_notifications();
    if (!s->get_notes().empty())
      my_service->notes = misc::string::check_string_utf8(s->get_notes());
    if (!s->get_notes_url().empty())
      my_service->notes_url =
          misc::string::check_string_utf8(s->get_notes_url());
    my_service->notifications_enabled = s->get_notifications_enabled();
    my_service->notification_interval = s->get_notification_interval();
    if (!s->get_notification_period().empty())
      my_service->notification_period = s->get_notification_period();
    my_service->notify_on_critical =
        s->get_notify_on(engine::notifier::critical);
    my_service->notify_on_downtime =
        s->get_notify_on(engine::notifier::downtime);
    my_service->notify_on_flapping =
        s->get_notify_on(engine::notifier::flappingstart);
    my_service->notify_on_recovery = s->get_notify_on(engine::notifier::ok);
    my_service->notify_on_unknown = s->get_notify_on(engine::notifier::unknown);
    my_service->notify_on_warning = s->get_notify_on(engine::notifier::warning);
    my_service->obsess_over = s->get_obsess_over();
    if (!s->get_plugin_output().empty()) {
      my_service->output =
          misc::string::check_string_utf8(s->get_plugin_output());
      my_service->output.append("\n");
    }
    if (!s->get_long_plugin_output().empty())
      my_service->output.append(
          misc::string::check_string_utf8(s->get_long_plugin_output()));
    my_service->passive_checks_enabled = s->get_accept_passive_checks();
    my_service->percent_state_change = s->get_percent_state_change();
    if (!s->get_perf_data().empty())
      my_service->perf_data =
          misc::string::check_string_utf8(s->get_perf_data());
    my_service->retain_nonstatus_information =
        s->get_retain_nonstatus_information();
    my_service->retain_status_information = s->get_retain_status_information();
    my_service->retry_interval = s->get_retry_interval();
    if (!s->get_description().empty())
      my_service->service_description =
          misc::string::check_string_utf8(s->get_description());
    my_service->should_be_scheduled = s->get_should_be_scheduled();
    my_service->stalk_on_critical = s->get_stalk_on(engine::notifier::critical);
    my_service->stalk_on_ok = s->get_stalk_on(engine::notifier::ok);
    my_service->stalk_on_unknown = s->get_stalk_on(engine::notifier::unknown);
    my_service->stalk_on_warning = s->get_stalk_on(engine::notifier::warning);
    my_service->state_type =
        (s->get_has_been_checked() ? s->get_state_type()
                                   : engine::notifier::hard);

    // Search host ID and service ID.
    std::pair<uint64_t, uint64_t> p;
    p = engine::get_host_and_service_id(s->get_hostname(),
                                        s->get_description());
    my_service->host_id = p.first;
    my_service->service_id = p.second;
    if (my_service->host_id && my_service->service_id) {
      // Send service event.
      logging::info(logging::low)
          << "callbacks: new service " << my_service->service_id << " ('"
          << my_service->service_description << "') on host "
          << my_service->host_id;
      neb::gl_publisher.write(my_service);

      /* No need to send this service custom variables changes, custom variables
       * are managed in a different loop. */
    } else
      logging::error(logging::medium)
          << "callbacks: service has no host ID or no service ID (yet) (host '"
          << (!s->get_hostname().empty() ? my_service->host_name : "(unknown)")
          << "', service '"
          << (!s->get_description().empty() ? my_service->service_description
                                            : "(unknown)")
          << "')";
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process service check data.
 *
 *  This function is called by Nagios when some service check data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_SERVICE_CHECK_DATA).
 *  @param[in] data          A pointer to a nebstruct_service_check_data
 *                           containing the service check data.
 *
 *  @return 0 on success.
 */
int neb::callback_service_check(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium) << "callbacks: generating service check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_service_check_data const* scdata;
    std::shared_ptr<neb::service_check> service_check(new neb::service_check);

    // Fill output var.
    scdata = static_cast<nebstruct_service_check_data*>(data);
    engine::service* s{static_cast<engine::service*>(scdata->object_ptr)};
    if (scdata->command_line) {
      service_check->active_checks_enabled = s->get_checks_enabled();
      service_check->check_type = scdata->check_type;
      service_check->command_line =
          misc::string::check_string_utf8(scdata->command_line);
      if (!scdata->host_id)
        throw exceptions::msg() << "host without id";
      if (!scdata->service_id)
        throw exceptions::msg() << "service without id";
      service_check->host_id = scdata->host_id;
      service_check->service_id = scdata->service_id;
      service_check->next_check = s->get_next_check();

      // Send event.
      gl_publisher.write(service_check);
    }
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating service check event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  @brief Function that process service status data.
 *
 *  This function is called by Nagios when some service status data are
 *  available.
 *
 *  @param[in] callback_type Type of the callback
 *                           (NEBCALLBACK_SERVICE_STATUS_DATA).
 *  @param[in] data          A pointer to a nebstruct_service_status_data
 *                           containing the service status data.
 *
 *  @return 0 on success.
 */
int neb::callback_service_status(int callback_type, void* data) {
  // Log message.
  logging::info(logging::medium)
      << "callbacks: generating service status event";
  (void)callback_type;

  try {
    // In/Out variables.
    std::shared_ptr<neb::service_status> service_status(
        new neb::service_status);

    // Fill output var.
    engine::service const* s{static_cast<engine::service*>(
        static_cast<nebstruct_service_status_data*>(data)->object_ptr)};
    service_status->acknowledged = s->get_problem_has_been_acknowledged();
    service_status->acknowledgement_type = s->get_acknowledgement_type();
    service_status->active_checks_enabled = s->get_checks_enabled();
    if (!s->get_check_command().empty())
      service_status->check_command =
          misc::string::check_string_utf8(s->get_check_command());
    service_status->check_interval = s->get_check_interval();
    if (!s->get_check_period().empty())
      service_status->check_period = s->get_check_period();
    service_status->check_type = s->get_check_type();
    service_status->current_check_attempt = s->get_current_attempt();
    service_status->current_state =
        (s->get_has_been_checked() ? s->get_current_state()
                                   : 4);  // Pending state.
    service_status->downtime_depth = s->get_scheduled_downtime_depth();
    if (!s->get_event_handler().empty())
      service_status->event_handler =
          misc::string::check_string_utf8(s->get_event_handler());
    service_status->event_handler_enabled = s->get_event_handler_enabled();
    service_status->execution_time = s->get_execution_time();
    service_status->flap_detection_enabled = s->get_flap_detection_enabled();
    service_status->has_been_checked = s->get_has_been_checked();
    service_status->is_flapping = s->get_is_flapping();
    service_status->last_check = s->get_last_check();
    service_status->last_hard_state = s->get_last_hard_state();
    service_status->last_hard_state_change = s->get_last_hard_state_change();
    service_status->last_notification = s->get_last_notification();
    service_status->notification_number = s->get_notification_number();
    service_status->last_state_change = s->get_last_state_change();
    service_status->last_time_critical = s->get_last_time_critical();
    service_status->last_time_ok = s->get_last_time_ok();
    service_status->last_time_unknown = s->get_last_time_unknown();
    service_status->last_time_warning = s->get_last_time_warning();
    service_status->last_update = time(nullptr);
    service_status->latency = s->get_latency();
    service_status->max_check_attempts = s->get_max_attempts();
    service_status->next_check = s->get_next_check();
    service_status->next_notification = s->get_next_notification();
    service_status->no_more_notifications = s->get_no_more_notifications();
    service_status->notifications_enabled = s->get_notifications_enabled();
    service_status->obsess_over = s->get_obsess_over();
    if (!s->get_plugin_output().empty()) {
      service_status->output =
          misc::string::check_string_utf8(s->get_plugin_output());
      service_status->output.append("\n");
    }
    if (!s->get_long_plugin_output().empty())
      service_status->output.append(
          misc::string::check_string_utf8(s->get_long_plugin_output()));

    service_status->passive_checks_enabled = s->get_accept_passive_checks();
    service_status->percent_state_change = s->get_percent_state_change();
    if (!s->get_perf_data().empty())
      service_status->perf_data =
          misc::string::check_string_utf8(s->get_perf_data());
    service_status->retry_interval = s->get_retry_interval();
    if (s->get_hostname().empty())
      throw exceptions::msg() << "unnamed host";
    if (s->get_description().empty())
      throw exceptions::msg() << "unnamed service";
    service_status->host_name =
        misc::string::check_string_utf8(s->get_hostname());
    service_status->service_description =
        misc::string::check_string_utf8(s->get_description());
    {
      std::pair<uint64_t, uint64_t> p{engine::get_host_and_service_id(
          s->get_hostname(), s->get_description())};
      service_status->host_id = p.first;
      service_status->service_id = p.second;
      if (!service_status->host_id || !service_status->service_id)
        throw exceptions::msg()
            << "could not find ID of service ('" << service_status->host_name
            << "', '" << service_status->service_description << "')";
    }
    service_status->should_be_scheduled = s->get_should_be_scheduled();
    service_status->state_type =
        (s->get_has_been_checked() ? s->get_state_type()
                                   : engine::notifier::hard);

    // Send event(s).
    gl_publisher.write(service_status);

    // Acknowledgement event.
    std::map<std::pair<uint32_t, uint32_t>, neb::acknowledgement>::iterator it(
        gl_acknowledgements.find(std::make_pair(service_status->host_id,
                                                service_status->service_id)));
    if ((it != gl_acknowledgements.end()) && !service_status->acknowledged) {
      if (!(!service_status->current_state  // !(OK or (normal ack and NOK))
            || (!it->second.is_sticky &&
                (service_status->current_state != it->second.state)))) {
        std::shared_ptr<neb::acknowledgement> ack(
            new neb::acknowledgement(it->second));
        ack->deletion_time = time(nullptr);
        gl_publisher.write(ack);
      }
      gl_acknowledgements.erase(it);
    }
  } catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
                                       " generating service status event: "
                                    << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {
  }
  return 0;
}

/**
 *  Unregister callbacks.
 */
void neb::unregister_callbacks() {
  gl_registered_callbacks.clear();
}
