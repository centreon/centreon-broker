/*
** Copyright 2009-2015 Centreon
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

#include <cstdlib>
#include <ctime>
#include <memory>
#include <QDomDocument>
#include <QDomElement>
#include <QString>
#include <QStringList>
#include <set>
#include <unistd.h>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/neb/callback.hh"
#include "com/centreon/broker/neb/callbacks.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/initial.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/set_log_data.hh"
#include "com/centreon/broker/neb/statistics/generator.hh"
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/globals.hh"
#include "com/centreon/engine/nebcallbacks.hh"
#include "com/centreon/engine/nebstructs.hh"
#include "com/centreon/engine/objects/hostdependency.hh"
#include "com/centreon/engine/objects/servicedependency.hh"
#include "com/centreon/engine/objects/comment.hh"
#include "com/centreon/engine/objects/hostgroup.hh"
#include "com/centreon/engine/objects/servicegroup.hh"
#include "com/centreon/engine/events/timed_event.hh"
#include "com/centreon/engine/events/defines.hh"

using namespace com::centreon::broker;

// List of Nagios modules.
extern nebmodule* neb_module_list;

// Load flags.
int neb::gl_mod_flags(0);

// Module handle.
void* neb::gl_mod_handle(NULL);

// List of common callbacks.
static struct {
  unsigned int macro;
  int (* callback)(int, void*);
} const gl_callbacks[] = {
  { NEBCALLBACK_EVENT_HANDLER_DATA, &neb::callback_event_handler },
  { NEBCALLBACK_EXTERNAL_COMMAND_DATA, &neb::callback_external_command },
  { NEBCALLBACK_FLAPPING_DATA, &neb::callback_flapping_status },
  { NEBCALLBACK_HOST_CHECK_DATA, &neb::callback_host_check },
  { NEBCALLBACK_HOST_STATUS_DATA, &neb::callback_host_status },
  { NEBCALLBACK_PROGRAM_STATUS_DATA, &neb::callback_program_status },
  { NEBCALLBACK_SERVICE_CHECK_DATA, &neb::callback_service_check },
  { NEBCALLBACK_SERVICE_STATUS_DATA, &neb::callback_service_status }
};

// List of Engine-specific callbacks.
static struct {
  unsigned int macro;
  int (* callback)(int, void*);
} const gl_engine_callbacks[] = {
  { NEBCALLBACK_ADAPTIVE_DEPENDENCY_DATA, &neb::callback_dependency },
  { NEBCALLBACK_ADAPTIVE_HOST_DATA, &neb::callback_host },
  { NEBCALLBACK_ADAPTIVE_SERVICE_DATA, &neb::callback_service },
  { NEBCALLBACK_CUSTOM_VARIABLE_DATA, &neb::callback_custom_variable },
  { NEBCALLBACK_GROUP_DATA, &neb::callback_group },
  { NEBCALLBACK_GROUP_MEMBER_DATA, &neb::callback_group_member },
  { NEBCALLBACK_MODULE_DATA, &neb::callback_module },
  { NEBCALLBACK_RELATION_DATA, &neb::callback_relation }
};

// Registered callbacks.
std::list<misc::shared_ptr<neb::callback> > neb::gl_registered_callbacks;

// Statistics generator.
static neb::statistics::generator gl_generator;

extern "C" void event_statistics(void* args) {
  (void)args;
  try {
    gl_generator.run();
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "stats: error occurred while generating statistics event: "
      << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return ;
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
    nebstruct_custom_variable_data const*
      cvar(static_cast<nebstruct_custom_variable_data*>(data));
    if (cvar && cvar->var_name && cvar->var_value) {
      // Host custom variable.
      if (NEBTYPE_HOSTCUSTOMVARIABLE_ADD == cvar->type) {
        ::host* hst(static_cast< ::host*>(cvar->object_ptr));
        if (hst && hst->name) {
          // Fill custom variable event.
          unsigned int host_id = engine::get_host_id(hst->name);
          if (host_id != 0) {
            misc::shared_ptr<custom_variable>
              new_cvar(new custom_variable);
            new_cvar->enabled = true;
            new_cvar->host_id = host_id;
            new_cvar->modified = false;
            new_cvar->name = cvar->var_name;
            new_cvar->var_type = 0;
            new_cvar->update_time = cvar->timestamp.tv_sec;
            new_cvar->value = cvar->var_value;

            // Send custom variable event.
            logging::info(logging::low)
              << "callbacks: new custom variable '" << new_cvar->name
              << "' on host " << new_cvar->host_id;
            neb::gl_publisher.write(new_cvar);
          }
        }
      }
      else if (NEBTYPE_HOSTCUSTOMVARIABLE_DELETE == cvar->type) {
        ::host* hst(static_cast< ::host*>(cvar->object_ptr));
        if (hst && hst->name) {
          unsigned int host_id = engine::get_host_id(hst->name);
          if (host_id != 0) {
            misc::shared_ptr<custom_variable>
              old_cvar(new custom_variable);
            old_cvar->enabled = false;
            old_cvar->host_id = host_id;
            old_cvar->name = cvar->var_name;
            old_cvar->var_type = 0;
            old_cvar->update_time = cvar->timestamp.tv_sec;

            // Send custom variable event.
            logging::info(logging::low)
              << "callbacks: deleted custom variable '"
              << old_cvar->name << "' on host '" << old_cvar->host_id;
            neb::gl_publisher.write(old_cvar);
          }
        }
      }
      // Service custom variable.
      else if (NEBTYPE_SERVICECUSTOMVARIABLE_ADD == cvar->type) {
        ::service* svc(static_cast< ::service*>(cvar->object_ptr));
        if (svc && svc->description && svc->host_name) {
          // Fill custom variable event.
          unsigned int host_id = engine::get_host_id(svc->host_name);
          unsigned int service_id = engine::get_service_id(
                                      svc->host_name,
                                      svc->description);
          if (host_id != 0 && service_id != 0) {
            misc::shared_ptr<custom_variable>
              new_cvar(new custom_variable);
            new_cvar->enabled = true;
            new_cvar->host_id = host_id;
            new_cvar->modified = false;
            new_cvar->name = cvar->var_name;
            new_cvar->service_id = service_id;
            new_cvar->var_type = 1;
            new_cvar->update_time = cvar->timestamp.tv_sec;
            new_cvar->value = cvar->var_value;

            // Send custom variable event.
            logging::info(logging::low)
              << "callbacks: new custom variable '" << new_cvar->name
              << "' on service (" << new_cvar->host_id << ", "
              << new_cvar->service_id << ")";
            neb::gl_publisher.write(new_cvar);
          }
        }
      }
      else if (NEBTYPE_SERVICECUSTOMVARIABLE_DELETE == cvar->type) {
        ::service* svc(static_cast< ::service*>(cvar->object_ptr));
        if (svc
            && svc->description
            && svc->host_name) {
          unsigned int host_id = engine::get_host_id(svc->host_name);
          unsigned int service_id = engine::get_service_id(
                                      svc->host_name,
                                      svc->description);
          if (host_id != 0 && service_id != 0) {
            misc::shared_ptr<custom_variable>
              old_cvar(new custom_variable);
            old_cvar->enabled = false;
            old_cvar->host_id = host_id;
            old_cvar->modified = true;
            old_cvar->name = cvar->var_name;
            old_cvar->service_id = service_id;
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
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating dependency event";
  (void)callback_type;

  try {
    // Input variables.
    nebstruct_adaptive_dependency_data*
      nsadd(static_cast<nebstruct_adaptive_dependency_data*>(data));

    // Host dependency.
    if ((NEBTYPE_HOSTDEPENDENCY_ADD == nsadd->type)
        || (NEBTYPE_HOSTDEPENDENCY_UPDATE == nsadd->type)
        || (NEBTYPE_HOSTDEPENDENCY_DELETE == nsadd->type)) {
      // Find IDs.
      unsigned int host_id;
      unsigned int dep_host_id;
      hostdependency*
        dep(static_cast<hostdependency*>(nsadd->object_ptr));
      if (dep->host_name) {
        host_id = engine::get_host_id(dep->host_name);
      }
      else {
        logging::error(logging::medium)
          << "callbacks: dependency callback called without "
          << "valid host";
        host_id = 0;
      }
      if (dep->dependent_host_name) {
        dep_host_id = engine::get_host_id(dep->dependent_host_name);
      }
      else {
        logging::error(logging::medium)
          << "callbacks: dependency callback called without "
          << "valid dependent host";
        dep_host_id = 0;
      }

      // Generate service dependency event.
      misc::shared_ptr<host_dependency>
        hst_dep(new host_dependency);
      hst_dep->host_id = host_id;
      hst_dep->dependent_host_id = dep_host_id;
      hst_dep->enabled = (nsadd->type != NEBTYPE_HOSTDEPENDENCY_DELETE);
      if (dep->dependency_period)
        hst_dep->dependency_period = dep->dependency_period;
      {
        QString options;
        if (dep->fail_on_down)
          options.append("d");
        if (dep->fail_on_up)
          options.append("o");
        if (dep->fail_on_pending)
          options.append("p");
        if (dep->fail_on_unreachable)
          options.append("u");
        if (dep->dependency_type == NOTIFICATION_DEPENDENCY)
          hst_dep->notification_failure_options = options;
        else if (dep->dependency_type == EXECUTION_DEPENDENCY)
          hst_dep->execution_failure_options = options;
      }
      hst_dep->inherits_parent = dep->inherits_parent;
      logging::info(logging::low) << "callbacks: host " << dep_host_id
        << " depends on host " << host_id;

      // Publish dependency event.
      neb::gl_publisher.write(hst_dep);
    }
    // Service dependency.
    else if ((NEBTYPE_SERVICEDEPENDENCY_ADD == nsadd->type)
             || (NEBTYPE_SERVICEDEPENDENCY_UPDATE == nsadd->type)
             || (NEBTYPE_SERVICEDEPENDENCY_DELETE == nsadd->type)) {
      // Find IDs.
      unsigned int host_id;
      unsigned int service_id;
      unsigned int dep_host_id;
      unsigned int dep_service_id;
      servicedependency*
        dep(static_cast<servicedependency*>(nsadd->object_ptr));
      if (dep->host_name && dep->service_description) {
        host_id = engine::get_host_id(dep->host_name);
        service_id = engine::get_service_id(
                       dep->host_name,
                       dep->service_description);
      }
      else {
        logging::error(logging::medium)
          << "callbacks: dependency callback called without "
          << "valid service";
        host_id = 0;
        service_id = 0;
      }
      if (dep->dependent_host_name
          && dep->dependent_service_description) {
        dep_host_id = engine::get_host_id(dep->host_name);
        dep_service_id = engine::get_service_id(
                           dep->host_name,
                           dep->service_description);
      }
      else {
        logging::error(logging::medium)
          << "callbacks: dependency callback called without "
          << "valid dependent service";
        dep_host_id = 0;
        dep_service_id = 0;
      }

      // Generate service dependency event.
      misc::shared_ptr<service_dependency>
        svc_dep(new service_dependency);
      svc_dep->host_id = host_id;
      svc_dep->service_id = service_id;
      svc_dep->dependent_host_id = dep_host_id;
      svc_dep->dependent_service_id = dep_service_id;
      svc_dep->enabled
        = (nsadd->type != NEBTYPE_SERVICEDEPENDENCY_DELETE);
      if (dep->dependency_period)
        svc_dep->dependency_period = dep->dependency_period;
      {
        QString options;
        if (dep->fail_on_critical)
          options.append("c");
        if (dep->fail_on_ok)
          options.append("o");
        if (dep->fail_on_pending)
          options.append("p");
        if (dep->fail_on_unknown)
          options.append("u");
        if (dep->fail_on_warning)
          options.append("w");
        if (dep->dependency_type == NOTIFICATION_DEPENDENCY)
          svc_dep->notification_failure_options = options;
        else if (dep->dependency_type == EXECUTION_DEPENDENCY)
          svc_dep->execution_failure_options = options;
      }
      svc_dep->inherits_parent = dep->inherits_parent;
      logging::info(logging::low) << "callbacks: service ("
        << dep_host_id << ", " << dep_service_id
        << ") depends on service (" << host_id << ", " << service_id
        << ")";

      // Publish dependency event.
      neb::gl_publisher.write(svc_dep);
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {}

  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating event handler event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_event_handler_data const* event_handler_data;
    misc::shared_ptr<neb::event_handler>
      event_handler(new neb::event_handler);

    // Fill output var.
    event_handler_data = static_cast<nebstruct_event_handler_data*>(data);
    if (event_handler_data->command_args)
      event_handler->command_args = event_handler_data->command_args;
    if (event_handler_data->command_line)
      event_handler->command_line = event_handler_data->command_line;
    event_handler->early_timeout = event_handler_data->early_timeout;
    event_handler->end_time = event_handler_data->end_time.tv_sec;
    event_handler->execution_time = event_handler_data->execution_time;
    if (!event_handler_data->host_name)
      throw (exceptions::msg() << "unnamed host");
    event_handler->host_id = engine::get_host_id(
                               event_handler_data->host_name);
    if (event_handler->host_id == 0)
      throw (exceptions::msg() << "could not find ID of host '"
             << event_handler_data->host_name << "'");
    if (event_handler_data->service_description) {
      event_handler->service_id = engine::get_service_id(
                                    event_handler_data->host_name,
                                    event_handler_data->service_description);
      if (event_handler->service_id == 0)
        throw (exceptions::msg() << "could not find ID of service ('"
               << event_handler_data->host_name << "', '"
               << event_handler_data->service_description << "')");
    }
    if (event_handler_data->output)
      event_handler->output = event_handler_data->output;
    event_handler->return_code = event_handler_data->return_code;
    event_handler->start_time = event_handler_data->start_time.tv_sec;
    event_handler->state = event_handler_data->state;
    event_handler->state_type = event_handler_data->state_type;
    event_handler->timeout = event_handler_data->timeout;
    event_handler->handler_type = event_handler_data->eventhandler_type;

    // Send event.
    gl_publisher.write(event_handler);
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating event handler event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
          QStringList l(QString(necd->command_args).split(';'));
          if (l.size() != 3)
            logging::error(logging::medium)
              << "callbacks: invalid host custom variable command";
          else {
            QStringList::iterator it(l.begin());
            QString host(*it++);
            QString var_name(*it++);
            QString var_value(*it);

            // Find host ID.
            unsigned int host_id = engine::get_host_id(
                                     host.toStdString().c_str());
            if (host_id != 0) {
              // Fill custom variable.
              misc::shared_ptr<neb::custom_variable_status>
                cvs(new neb::custom_variable_status);
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
      }
      else if (necd->command_type == CMD_CHANGE_CUSTOM_SVC_VAR) {
        logging::info(logging::medium)
          << "callbacks: generating service custom variable update event";

        // Split argument string.
        if (necd->command_args) {
          QStringList l(QString(necd->command_args).split(';'));
          if (l.size() != 4)
            logging::error(logging::medium)
              << "callbacks: invalid service custom variable command";
          else {
            QStringList::iterator it(l.begin());
            QString host(*it++);
            QString service(*it++);
            QString var_name(*it++);
            QString var_value(*it);

            // Find host/service IDs.
            unsigned int host_id = engine::get_host_id(
                                     host.toStdString().c_str());
            unsigned int service_id = engine::get_service_id(
                                        host.toStdString().c_str(),
                                        service.toStdString().c_str());
            if (host_id != 0 && service_id != 0) {
              // Fill custom variable.
              misc::shared_ptr<neb::custom_variable_status> cvs(
                new neb::custom_variable_status);
              cvs->host_id = host_id;
              cvs->modified = true;
              cvs->name = var_name;
              cvs->service_id = service_id;
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
    catch (...) {}
  }
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating flapping event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_flapping_data const* flapping_data;
    misc::shared_ptr<neb::flapping_status> flapping_status(new neb::flapping_status);

    // Fill output var.
    flapping_data = static_cast<nebstruct_flapping_data*>(data);
    flapping_status->event_time = flapping_data->timestamp.tv_sec;
    flapping_status->event_type = flapping_data->type;
    flapping_status->high_threshold = flapping_data->high_threshold;
    if (!flapping_data->host_name)
      throw (exceptions::msg() << "unnamed host");
    flapping_status->host_id = engine::get_host_id(flapping_data->host_name);
    if (flapping_status->host_id == 0)
      throw (exceptions::msg() << "could not find ID of host '"
             << flapping_data->host_name << "'");
    if (flapping_data->service_description) {
      flapping_status->service_id = engine::get_service_id(
                                      flapping_data->host_name,
                                      flapping_data->service_description);
      if (flapping_status->service_id == 0)
        throw (exceptions::msg() << "could not find ID of service ('"
               << flapping_data->host_name << "', '"
               << flapping_data->service_description << "')");
    }
    flapping_status->low_threshold = flapping_data->low_threshold;
    flapping_status->percent_state_change = flapping_data->percent_change;
    // flapping_status->reason_type = XXX;
    flapping_status->flapping_type = flapping_data->flapping_type;

    // Send event.
    gl_publisher.write(flapping_status);
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      "generating flapping event: " << e.what();
  }
  // Avoid exception propagation to C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating group event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_group_data const*
      group_data(static_cast<nebstruct_group_data*>(data));

    // Host group.
    if ((NEBTYPE_HOSTGROUP_ADD == group_data->type)
        || (NEBTYPE_HOSTGROUP_UPDATE == group_data->type)
        || (NEBTYPE_HOSTGROUP_DELETE == group_data->type)) {
      ::hostgroup const*
        host_group(static_cast< ::hostgroup*>(group_data->object_ptr));
      if (host_group->group_name) {
        misc::shared_ptr<neb::host_group> new_hg(new neb::host_group);
        new_hg->poller_id
          = config::applier::state::instance().poller_id();
        new_hg->id = engine::get_hostgroup_id(host_group->group_name);
        new_hg->enabled
          = ((group_data->type != NEBTYPE_HOSTGROUP_DELETE)
             && host_group->members);
        new_hg->name = host_group->group_name;

        // Send host group event.
        if (new_hg->id) {
          logging::info(logging::low) << "callbacks: new host group "
            << new_hg->id << " ('" << new_hg->name << "') on instance "
            << new_hg->poller_id;
          neb::gl_publisher.write(new_hg);
        }
      }
    }
    // Service group.
    else if ((NEBTYPE_SERVICEGROUP_ADD == group_data->type)
             || (NEBTYPE_SERVICEGROUP_UPDATE == group_data->type)
             || (NEBTYPE_SERVICEGROUP_DELETE == group_data->type)) {
      ::servicegroup const*
        service_group(static_cast< ::servicegroup*>(group_data->object_ptr));
      if (service_group->group_name) {
        misc::shared_ptr<neb::service_group>
          new_sg(new neb::service_group);
        new_sg->poller_id
          = config::applier::state::instance().poller_id();
        new_sg->id
          = engine::get_servicegroup_id(service_group->group_name);
        new_sg->enabled
          = ((group_data->type != NEBTYPE_SERVICEGROUP_DELETE)
             && service_group->members);
        new_sg->name = service_group->group_name;

        // Send service group event.
        if (new_sg->id) {
          logging::info(logging::low) << "callbacks:: new service group "
            << new_sg->id << " ('" << new_sg->name << "') on instance "
            << new_sg->poller_id;
          neb::gl_publisher.write(new_sg);
        }
      }
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating group member event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_group_member_data const*
      member_data(static_cast<nebstruct_group_member_data*>(data));

    // Host group member.
    if ((member_data->type == NEBTYPE_HOSTGROUPMEMBER_ADD)
        || (member_data->type == NEBTYPE_HOSTGROUPMEMBER_DELETE)) {
      ::host const*
        hst(static_cast< ::host*>(member_data->object_ptr));
      ::hostgroup const*
        hg(static_cast< ::hostgroup*>(member_data->group_ptr));
      if (hst->name && hg->group_name) {
        // Output variable.
        misc::shared_ptr<neb::host_group_member>
          hgm(new neb::host_group_member);
        hgm->group_id = engine::get_hostgroup_id(hg->group_name);
        hgm->group_name = hg->group_name;
        hgm->poller_id = config::applier::state::instance().poller_id();
        unsigned int host_id = engine::get_host_id(hst->name);
        if (host_id != 0 && hgm->group_id != 0) {
          hgm->host_id = host_id;
          if (member_data->type == NEBTYPE_HOSTGROUPMEMBER_DELETE) {
            logging::info(logging::low) << "callbacks: host "
              << hgm->host_id << " is not a member of group "
              << hgm->group_id << " on instance " << hgm->poller_id
              << " anymore";
            hgm->enabled = false;
          }
          else {
            logging::info(logging::low) << "callbacks: host "
              << hgm->host_id << " is a member of group "
              << hgm->group_id << " on instance " << hgm->poller_id;
            hgm->enabled = true;
          }

          // Send host group member event.
          if (hgm->host_id && hgm->group_id)
            neb::gl_publisher.write(hgm);
        }
      }
    }
    // Service group member.
    else if ((member_data->type == NEBTYPE_SERVICEGROUPMEMBER_ADD)
             || (member_data->type == NEBTYPE_SERVICEGROUPMEMBER_DELETE)) {
      ::service const*
        svc(static_cast< ::service*>(member_data->object_ptr));
      ::servicegroup const*
        sg(static_cast< ::servicegroup*>(member_data->group_ptr));
      if (svc->description
          && sg->group_name
          && svc->host_name) {
        // Output variable.
        misc::shared_ptr<neb::service_group_member>
          sgm(new neb::service_group_member);
        sgm->group_id = engine::get_servicegroup_id(sg->group_name);
        sgm->group_name = sg->group_name;
        sgm->poller_id = config::applier::state::instance().poller_id();
        sgm->host_id = engine::get_host_id(svc->host_name);
        sgm->service_id = engine::get_service_id(
                            svc->host_name,
                            svc->description);
        if (sgm->host_id && sgm->service_id && sgm->group_id) {
          if (member_data->type == NEBTYPE_SERVICEGROUPMEMBER_DELETE) {
            logging::info(logging::low) << "callbacks: service ("
              << sgm->host_id << ", " << sgm->service_id
              << ") is not a member of group " << sgm->group_id
              << " on instance " << sgm->poller_id << " anymore";
            sgm->enabled = false;
          }
          else {
            logging::info(logging::low) << "callbacks: service ("
              << sgm->host_id << ", " << sgm->service_id
              << ") is a member of group " << sgm->group_id
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
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating host event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_adaptive_host_data const*
      host_data(static_cast<nebstruct_adaptive_host_data*>(data));
    ::host const*
      h(static_cast< ::host*>(host_data->object_ptr));
    misc::shared_ptr<neb::host> my_host(new neb::host);

    // Set host parameters.
    my_host->acknowledged = h->problem_has_been_acknowledged;
    my_host->acknowledgement_type = h->acknowledgement_type;
    if (h->action_url)
      my_host->action_url = h->action_url;
    my_host->active_checks_enabled = h->checks_enabled;
    if (h->address)
      my_host->address = h->address;
    if (h->alias)
      my_host->alias = h->alias;
    my_host->check_freshness = h->check_freshness;
    if (h->host_check_command)
      my_host->check_command = h->host_check_command;
    my_host->check_interval = h->check_interval;
    if (h->check_period)
      my_host->check_period = h->check_period;
    my_host->check_type = h->check_type;
    my_host->current_check_attempt = h->current_attempt;
    my_host->current_state = (h->has_been_checked
                              ? h->current_state
                              : 4); // Pending state.
    my_host->default_active_checks_enabled = h->checks_enabled;
    my_host->default_event_handler_enabled = h->event_handler_enabled;
    my_host->default_flap_detection_enabled = h->flap_detection_enabled;
    my_host->default_notifications_enabled = h->notifications_enabled;
    my_host->default_passive_checks_enabled = h->accept_passive_host_checks;
    my_host->enabled = (host_data->type != NEBTYPE_HOST_DELETE);
    if (h->event_handler)
      my_host->event_handler = h->event_handler;
    my_host->event_handler_enabled = h->event_handler_enabled;
    my_host->execution_time = h->execution_time;
    my_host->first_notification_delay = h->first_notification_delay;
    my_host->flap_detection_enabled = h->flap_detection_enabled;
    my_host->flap_detection_on_down = h->flap_detection_on_down;
    my_host->flap_detection_on_unreachable
      = h->flap_detection_on_unreachable;
    my_host->flap_detection_on_up = h->flap_detection_on_up;
    my_host->freshness_threshold = h->freshness_threshold;
    my_host->has_been_checked = h->has_been_checked;
    my_host->high_flap_threshold = h->high_flap_threshold;
    if (h->name)
      my_host->host_name = h->name;
    if (h->icon_image)
      my_host->icon_image = h->icon_image;
    if (h->icon_image_alt)
      my_host->icon_image_alt = h->icon_image_alt;
    my_host->is_flapping = h->is_flapping;
    my_host->last_check = h->last_check;
    my_host->last_hard_state = h->last_hard_state;
    my_host->last_hard_state_change = h->last_hard_state_change;
    my_host->last_notification = h->last_host_notification;
    my_host->last_state_change = h->last_state_change;
    my_host->last_time_down = h->last_time_down;
    my_host->last_time_unreachable = h->last_time_unreachable;
    my_host->last_time_up = h->last_time_up;
    my_host->last_update = time(NULL);
    my_host->latency = h->latency;
    my_host->low_flap_threshold = h->low_flap_threshold;
    my_host->max_check_attempts = h->max_attempts;
    my_host->next_check = h->next_check;
    my_host->next_notification = h->next_host_notification;
    my_host->no_more_notifications = h->no_more_notifications;
    if (h->notes)
      my_host->notes = h->notes;
    if (h->notes_url)
      my_host->notes_url = h->notes_url;
    my_host->notifications_enabled = h->notifications_enabled;
    my_host->notification_interval = h->notification_interval;
    if (h->notification_period)
      my_host->notification_period = h->notification_period;
    my_host->notify_on_down = h->notify_on_down;
    my_host->notify_on_downtime = h->notify_on_downtime;
    my_host->notify_on_flapping = h->notify_on_flapping;
    my_host->notify_on_recovery = h->notify_on_recovery;
    my_host->notify_on_unreachable = h->notify_on_unreachable;
    my_host->obsess_over = h->obsess_over_host;
    if (h->plugin_output) {
      my_host->output = h->plugin_output;
      my_host->output.append("\n");
    }
    if (h->long_plugin_output)
        my_host->output.append(h->long_plugin_output);
    my_host->passive_checks_enabled = h->accept_passive_host_checks;
    my_host->percent_state_change = h->percent_state_change;
    if (h->perf_data)
      my_host->perf_data = h->perf_data;
    my_host->poller_id = config::applier::state::instance().poller_id();
    my_host->retain_nonstatus_information
      = h->retain_nonstatus_information;
    my_host->retain_status_information = h->retain_status_information;
    my_host->retry_interval = h->retry_interval;
    my_host->should_be_scheduled = h->should_be_scheduled;
    my_host->stalk_on_down = h->stalk_on_down;
    my_host->stalk_on_unreachable = h->stalk_on_unreachable;
    my_host->stalk_on_up = h->stalk_on_up;
    my_host->state_type = (h->has_been_checked
                           ? h->state_type
                           : HARD_STATE);
    if (h->statusmap_image)
      my_host->statusmap_image = h->statusmap_image;

    // Find host ID.
    unsigned int host_id = engine::get_host_id(
                             my_host->host_name.toStdString().c_str());
    if (host_id != 0) {
      my_host->host_id = host_id;

      // Send host event.
      logging::info(logging::low) << "callbacks:  new host "
        << my_host->host_id << " ('" << my_host->host_name
        << "') on instance " << my_host->poller_id;
      neb::gl_publisher.write(my_host);

      // Generate existing custom variables.
      for (customvariablesmember* cvar(h->custom_variables);
           cvar;
           cvar = cvar->next)
        if (cvar->variable_name
            && strcmp(cvar->variable_name, "HOST_ID")) {
          nebstruct_custom_variable_data data;
          memset(&data, 0, sizeof(data));
          data.type = NEBTYPE_HOSTCUSTOMVARIABLE_ADD;
          data.timestamp.tv_sec = host_data->timestamp.tv_sec;
          data.var_name = cvar->variable_name;
          data.var_value = cvar->variable_value;
          data.object_ptr = host_data->object_ptr;
          callback_custom_variable(
            NEBCALLBACK_CUSTOM_VARIABLE_DATA,
            &data);
        }
    }
    else
      logging::error(logging::medium) << "callbacks: host '"
        << (h->name ? h->name : "(unknown)")
        << "' has no ID (yet) defined";
  }
  // Avoid exception propagation to C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating host check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_host_check_data const* hcdata;
    misc::shared_ptr<neb::host_check> host_check(new neb::host_check);

    // Fill output var.
    hcdata = static_cast<nebstruct_host_check_data*>(data);
    ::host* h(static_cast< ::host*>(hcdata->object_ptr));
    if (hcdata->command_line) {
      host_check->active_checks_enabled = h->checks_enabled;
      host_check->check_type = hcdata->check_type;
      host_check->command_line = hcdata->command_line;
      if (!hcdata->host_name)
        throw (exceptions::msg() << "unnamed host");
      host_check->host_id = engine::get_host_id(hcdata->host_name);
      if (host_check->host_id == 0)
        throw (exceptions::msg() << "could not find ID of host '"
               << hcdata->host_name << "'");
      host_check->next_check = h->next_check;

      // Send event.
      gl_publisher.write(host_check);
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating host check event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating host status event";
  (void)callback_type;

  try {
    // In/Out variables.
    ::host const* h;
    misc::shared_ptr<neb::host_status> host_status(new neb::host_status);

    // Fill output var.
    h = static_cast< ::host*>(
      static_cast<nebstruct_host_status_data*>(data)->object_ptr);
    host_status->acknowledged = h->problem_has_been_acknowledged;
    host_status->acknowledgement_type = h->acknowledgement_type;
    host_status->active_checks_enabled = h->checks_enabled;
    if (h->host_check_command)
      host_status->check_command = h->host_check_command;
    host_status->check_interval = h->check_interval;
    if (h->check_period)
      host_status->check_period = h->check_period;
    host_status->check_type = h->check_type;
    host_status->current_check_attempt = h->current_attempt;
    host_status->current_state = (h->has_been_checked
                                  ? h->current_state
                                  : 4); // Pending state.
    if (h->event_handler)
      host_status->event_handler = h->event_handler;
    host_status->event_handler_enabled = h->event_handler_enabled;
    host_status->execution_time = h->execution_time;
    host_status->flap_detection_enabled = h->flap_detection_enabled;
    host_status->has_been_checked = h->has_been_checked;
    if (!h->name)
      throw (exceptions::msg() << "unnamed host");
    {
      host_status->host_id = engine::get_host_id(h->name);
      if (host_status->host_id == 0)
        throw (exceptions::msg() << "could not find ID of host '"
               << h->name << "'");
    }
    host_status->is_flapping = h->is_flapping;
    host_status->last_check = h->last_check;
    host_status->last_hard_state = h->last_hard_state;
    host_status->last_hard_state_change = h->last_hard_state_change;
    host_status->last_notification = h->last_host_notification;
    host_status->last_state_change = h->last_state_change;
    host_status->last_time_down = h->last_time_down;
    host_status->last_time_unreachable = h->last_time_unreachable;
    host_status->last_time_up = h->last_time_up;
    host_status->last_update = time(NULL);
    host_status->latency = h->latency;
    host_status->max_check_attempts = h->max_attempts;
    host_status->next_check = h->next_check;
    host_status->next_notification = h->next_host_notification;
    host_status->no_more_notifications = h->no_more_notifications;
    host_status->notifications_enabled = h->notifications_enabled;
    host_status->obsess_over = h->obsess_over_host;
    if (h->plugin_output) {
      host_status->output = h->plugin_output;
      host_status->output.append("\n");
    }
    if (h->long_plugin_output)
      host_status->output.append(h->long_plugin_output);
    host_status->passive_checks_enabled = h->accept_passive_host_checks;
    host_status->percent_state_change = h->percent_state_change;
    if (h->perf_data)
      host_status->perf_data = h->perf_data;
    host_status->retry_interval = h->retry_interval;
    host_status->should_be_scheduled = h->should_be_scheduled;
    host_status->state_type = (h->has_been_checked
                               ? h->state_type
                               : HARD_STATE);

    // Send event(s).
    gl_publisher.write(host_status);
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating host status event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
    misc::shared_ptr<neb::log_entry> le(new neb::log_entry);

    // Fill output var.
    log_data = static_cast<nebstruct_log_data*>(data);
    le->c_time = log_data->entry_time;
    le->poller_name
      = config::applier::state::instance().poller_name().c_str();
    if (log_data->data) {
      if (log_data->data)
        le->output = log_data->data;
      set_log_data(*le, log_data->data);
    }

    // Send event.
    gl_publisher.write(le);
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
    misc::shared_ptr<neb::module> me(new neb::module);

    // Fill output var.
    module_data = static_cast<nebstruct_module_data*>(data);
    if (module_data->module) {
      me->poller_id = config::applier::state::instance().poller_id();
      me->filename = module_data->module;
      if (module_data->args)
        me->args = module_data->args;
      me->loaded = !(module_data->type == NEBTYPE_MODULE_DELETE);
      me->should_be_loaded = true;

      // Send events.
      gl_publisher.write(me);
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
int neb::callback_process(int callback_type, void *data) {
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
      logging::debug(logging::high)
        << "callbacks: registering callbacks";
      for (unsigned int i(0);
           i < sizeof(gl_callbacks) / sizeof(*gl_callbacks);
           ++i)
        gl_registered_callbacks.push_back(
          misc::shared_ptr<callback>(new neb::callback(
                                         gl_callbacks[i].macro,
                                         gl_mod_handle,
                                         gl_callbacks[i].callback)));

      // Register Engine-specific callbacks.
      if (gl_mod_flags & NEBMODULE_ENGINE) {
        for (unsigned int i(0);
             i < sizeof(gl_engine_callbacks) / sizeof(*gl_engine_callbacks);
             ++i)
          gl_registered_callbacks.push_back(
            misc::shared_ptr<callback>(new neb::callback(
                                           gl_engine_callbacks[i].macro,
                                           gl_mod_handle,
                                           gl_engine_callbacks[i].callback)));
      }

      // Parse configuration file.
      unsigned int statistics_interval(0);
      try {
        config::parser parsr;
        config::state conf;
        parsr.parse(gl_configuration_file, conf);

        // Apply resulting configuration.
        config::applier::state::instance().apply(conf);
        gl_generator.set(conf);

        // Set variables.
        statistics_interval = gl_generator.interval();
      }
      catch (exceptions::msg const& e) {
        logging::config(logging::high) << e.what();
        return (0);
      }

      // Output variable.
      misc::shared_ptr<neb::instance> instance(new neb::instance);
      instance->poller_id
        = config::applier::state::instance().poller_id();
      instance->engine = "Centreon Engine";
      instance->is_running = true;
      instance->name
        = config::applier::state::instance().poller_name().c_str();
      instance->pid = getpid();
      instance->program_start = time(NULL);
      instance->version = "2.x";
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
          void (* code)(void*);
          void* data;
        } val;
        val.code = &event_statistics;
        schedule_new_event(
          EVENT_USER_FUNCTION,
          0,
          time(NULL) + statistics_interval,
          1,
          statistics_interval,
          NULL,
          1,
          val.data,
          NULL,
          0);
      }
    }
    else if (NEBTYPE_PROCESS_EVENTLOOPEND == process_data->type) {
      logging::info(logging::medium)
        << "callbacks: generating process end event";
      // Output variable.
      misc::shared_ptr<neb::instance> instance(new neb::instance);

      // Fill output var.
      instance->poller_id
        = config::applier::state::instance().poller_id();
      instance->engine = "Centreon Engine";
      instance->is_running = false;
      instance->name
        = config::applier::state::instance().poller_name().c_str();
      instance->pid = getpid();
      instance->program_end = time(NULL);
      instance->program_start = start_time;
      instance->version = "2.x";

      // Send event.
      gl_publisher.write(instance);
    }
  }
  // Avoid exception propagation in C code.
  catch (...) {
    unregister_callbacks();
  }
  return (0);
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
    misc::shared_ptr<neb::instance_status> is(
      new neb::instance_status);

    // Fill output var.
    program_status_data = static_cast<nebstruct_program_status_data*>(data);
    is->poller_id = config::applier::state::instance().poller_id();
    is->active_host_checks_enabled
      = program_status_data->active_host_checks_enabled;
    is->active_service_checks_enabled
      = program_status_data->active_service_checks_enabled;
    is->check_hosts_freshness = check_host_freshness;
    is->check_services_freshness = check_service_freshness;
    is->event_handler_enabled
      = program_status_data->event_handlers_enabled;
    is->flap_detection_enabled
      = program_status_data->flap_detection_enabled;
    if (program_status_data->global_host_event_handler)
      is->global_host_event_handler
        = program_status_data->global_host_event_handler;
    if (program_status_data->global_service_event_handler)
      is->global_service_event_handler
        = program_status_data->global_service_event_handler;
    is->last_alive = time(NULL);
    is->last_command_check = program_status_data->last_command_check;
    is->notifications_enabled
      = program_status_data->notifications_enabled;
    is->obsess_over_hosts
      = program_status_data->obsess_over_hosts;
    is->obsess_over_services
      = program_status_data->obsess_over_services;
    is->passive_host_checks_enabled
      = program_status_data->passive_host_checks_enabled;
    is->passive_service_checks_enabled
      = program_status_data->passive_service_checks_enabled;

    // Send event.
    gl_publisher.write(is);
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating relation event";
  (void)callback_type;

  try {
    // Input variable.
    nebstruct_relation_data const*
      relation(static_cast<nebstruct_relation_data*>(data));

    // Host parent.
    if ((NEBTYPE_PARENT_ADD == relation->type)
        || (NEBTYPE_PARENT_DELETE == relation->type)) {
      if (relation->hst
          && relation->dep_hst
          && !relation->svc
          && !relation->dep_svc) {
        // Find host IDs.
        int host_id;
        int parent_id;
        {
          host_id = engine::get_host_id(relation->dep_hst->name);
          parent_id = engine::get_host_id(relation->hst->name);
        }
        if (host_id && parent_id) {
          // Generate parent event.
          misc::shared_ptr<host_parent> new_host_parent(new host_parent);
          new_host_parent->enabled
            = (relation->type != NEBTYPE_PARENT_DELETE);
          new_host_parent->host_id = host_id;
          new_host_parent->parent_id = parent_id;

          // Send event.
          logging::info(logging::low) << "callbacks: host "
            << new_host_parent->parent_id << " is parent of host "
            << new_host_parent->host_id;
          neb::gl_publisher.write(
                              new_host_parent);
        }
      }
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating service event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_adaptive_service_data const*
      service_data(static_cast<nebstruct_adaptive_service_data*>(data));
    ::service const*
      s(static_cast< ::service*>(service_data->object_ptr));
    misc::shared_ptr<neb::service> my_service(new neb::service);

    // Fill output var.
    my_service->acknowledged = s->problem_has_been_acknowledged;
    my_service->acknowledgement_type = s->acknowledgement_type;
    if (s->action_url)
      my_service->action_url = s->action_url;
    my_service->active_checks_enabled = s->checks_enabled;
    if (s->service_check_command)
      my_service->check_command = s->service_check_command;
    my_service->check_freshness = s->check_freshness;
    my_service->check_interval = s->check_interval;
    if (s->check_period)
      my_service->check_period = s->check_period;
    my_service->check_type = s->check_type;
    my_service->current_check_attempt = s->current_attempt;
    my_service->current_state = (s->has_been_checked
                                 ? s->current_state
                                 : 4); // Pending state.
    my_service->default_active_checks_enabled = s->checks_enabled;
    my_service->default_event_handler_enabled = s->event_handler_enabled;
    my_service->default_flap_detection_enabled = s->flap_detection_enabled;
    my_service->default_notifications_enabled = s->notifications_enabled;
    my_service->default_passive_checks_enabled
      = s->accept_passive_service_checks;
    my_service->enabled
      = (service_data->type != NEBTYPE_SERVICE_DELETE);
    if (s->event_handler)
      my_service->event_handler = s->event_handler;
    my_service->event_handler_enabled = s->event_handler_enabled;
    my_service->execution_time = s->execution_time;
    my_service->first_notification_delay = s->first_notification_delay;
    my_service->flap_detection_enabled = s->flap_detection_enabled;
    my_service->flap_detection_on_critical = s->flap_detection_on_critical;
    my_service->flap_detection_on_ok = s->flap_detection_on_ok;
    my_service->flap_detection_on_unknown = s->flap_detection_on_unknown;
    my_service->flap_detection_on_warning = s->flap_detection_on_warning;
    my_service->freshness_threshold = s->freshness_threshold;
    my_service->has_been_checked = s->has_been_checked;
    my_service->high_flap_threshold = s->high_flap_threshold;
    if (s->host_name)
      my_service->host_name = s->host_name;
    if (s->icon_image)
      my_service->icon_image = s->icon_image;
    if (s->icon_image_alt)
      my_service->icon_image_alt = s->icon_image_alt;
    my_service->is_flapping = s->is_flapping;
    my_service->is_volatile = s->is_volatile;
    my_service->last_check = s->last_check;
    my_service->last_hard_state = s->last_hard_state;
    my_service->last_hard_state_change = s->last_hard_state_change;
    my_service->last_notification = s->last_notification;
    my_service->last_state_change = s->last_state_change;
    my_service->last_time_critical = s->last_time_critical;
    my_service->last_time_ok = s->last_time_ok;
    my_service->last_time_unknown = s->last_time_unknown;
    my_service->last_time_warning = s->last_time_warning;
    my_service->last_update = time(NULL);
    my_service->latency = s->latency;
    my_service->low_flap_threshold = s->low_flap_threshold;
    my_service->max_check_attempts = s->max_attempts;
    my_service->next_check = s->next_check;
    my_service->next_notification = s->next_notification;
    my_service->no_more_notifications = s->no_more_notifications;
    if (s->notes)
      my_service->notes = s->notes;
    if (s->notes_url)
      my_service->notes_url = s->notes_url;
    my_service->notifications_enabled = s->notifications_enabled;
    my_service->notification_interval = s->notification_interval;
    if (s->notification_period)
      my_service->notification_period = s->notification_period;
    my_service->notify_on_critical = s->notify_on_critical;
    my_service->notify_on_downtime = s->notify_on_downtime;
    my_service->notify_on_flapping = s->notify_on_flapping;
    my_service->notify_on_recovery = s->notify_on_recovery;
    my_service->notify_on_unknown = s->notify_on_unknown;
    my_service->notify_on_warning = s->notify_on_warning;
    my_service->obsess_over = s->obsess_over_service;
    if (s->plugin_output) {
      my_service->output = s->plugin_output;
      my_service->output.append("\n");
    }
    if (s->long_plugin_output)
      my_service->output.append(s->long_plugin_output);
    my_service->passive_checks_enabled
      = s->accept_passive_service_checks;
    my_service->percent_state_change = s->percent_state_change;
    if (s->perf_data)
      my_service->perf_data = s->perf_data;
    my_service->retain_nonstatus_information
      = s->retain_nonstatus_information;
    my_service->retain_status_information
      = s->retain_status_information;
    my_service->retry_interval = s->retry_interval;
    if (s->description)
      my_service->service_description = s->description;
    my_service->should_be_scheduled = s->should_be_scheduled;
    my_service->stalk_on_critical = s->stalk_on_critical;
    my_service->stalk_on_ok = s->stalk_on_ok;
    my_service->stalk_on_unknown = s->stalk_on_unknown;
    my_service->stalk_on_warning = s->stalk_on_warning;
    my_service->state_type = (s->has_been_checked
                              ? s->state_type
                              : HARD_STATE);

    // Search host ID and service ID.
    my_service->host_id = engine::get_host_id(
                            s->host_name ? s->host_name : "");
    my_service->service_id = engine::get_service_id(
                               s->host_name ? s->host_name : "",
                               my_service->service_description.toStdString().c_str());
    if (my_service->host_id && my_service->service_id) {
      // Send service event.
      logging::info(logging::low) << "callbacks: new service "
        << my_service->service_id << " ('"
        << my_service->service_description
        << "') on host " << my_service->host_id;
      neb::gl_publisher.write(my_service);

      // Generate existing custom variables.
      for (customvariablesmember* cvar(s->custom_variables);
           cvar;
           cvar = cvar->next)
        if (cvar->variable_name
            && strcmp(cvar->variable_name, "HOST_ID")
            && strcmp(cvar->variable_name, "SERVICE_ID")) {
          nebstruct_custom_variable_data data;
          memset(&data, 0, sizeof(data));
          data.type = NEBTYPE_SERVICECUSTOMVARIABLE_ADD;
          data.timestamp.tv_sec = service_data->timestamp.tv_sec;
          data.var_name = cvar->variable_name;
          data.var_value = cvar->variable_value;
          data.object_ptr = service_data->object_ptr;
          callback_custom_variable(
            NEBCALLBACK_CUSTOM_VARIABLE_DATA,
            &data);
        }
    }
    else
      logging::error(logging::medium)
        << "callbacks: service has no host ID or no service ID (yet) (host '"
        << (s->host_name ? s->host_name : "(unknown)")
        << "', service '"
        << (s->description ? s->description : "(unknown)")
        << "')";
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating service check event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_service_check_data const* scdata;
    misc::shared_ptr<neb::service_check> service_check(
      new neb::service_check);

    // Fill output var.
    scdata = static_cast<nebstruct_service_check_data*>(data);
    ::service* s(static_cast< ::service*>(scdata->object_ptr));
    if (scdata->command_line) {
      service_check->active_checks_enabled = s->checks_enabled;
      service_check->check_type = scdata->check_type;
      service_check->command_line = scdata->command_line;
      if (!scdata->host_name)
        throw (exceptions::msg() << "unnamed host");
      if (!scdata->service_description)
        throw (exceptions::msg() << "unnamed service");
      service_check->host_id = engine::get_host_id(scdata->host_name);
      service_check->service_id = engine::get_service_id(
                                    scdata->host_name,
                                    scdata->service_description);
      if (!service_check->host_id || !service_check->service_id)
        throw (exceptions::msg() << "could not find ID of service ('"
               << scdata->host_name << "', '"
               << scdata->service_description << "')");
      service_check->next_check = s->next_check;

      // Send event.
      gl_publisher.write(service_check);
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating service check event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
    ::service const* s;
    misc::shared_ptr<neb::service_status> service_status(
      new neb::service_status);

    // Fill output var.
    s = static_cast< ::service*>(
      static_cast<nebstruct_service_status_data*>(data)->object_ptr);
    service_status->acknowledged = s->problem_has_been_acknowledged;
    service_status->acknowledgement_type = s->acknowledgement_type;
    service_status->active_checks_enabled = s->checks_enabled;
    if (s->service_check_command)
      service_status->check_command = s->service_check_command;
    service_status->check_interval = s->check_interval;
    if (s->check_period)
      service_status->check_period = s->check_period;
    service_status->check_type = s->check_type;
    service_status->current_check_attempt = s->current_attempt;
    service_status->current_state = (s->has_been_checked
                                     ? s->current_state
                                     : 4); // Pending state.
    if (s->event_handler)
      service_status->event_handler = s->event_handler;
    service_status->event_handler_enabled = s->event_handler_enabled;
    service_status->execution_time = s->execution_time;
    service_status->flap_detection_enabled = s->flap_detection_enabled;
    service_status->has_been_checked = s->has_been_checked;
    service_status->is_flapping = s->is_flapping;
    service_status->last_check = s->last_check;
    service_status->last_hard_state = s->last_hard_state;
    service_status->last_hard_state_change = s->last_hard_state_change;
    service_status->last_notification = s->last_notification;
    service_status->last_state_change = s->last_state_change;
    service_status->last_time_critical = s->last_time_critical;
    service_status->last_time_ok = s->last_time_ok;
    service_status->last_time_unknown = s->last_time_unknown;
    service_status->last_time_warning = s->last_time_warning;
    service_status->last_update = time(NULL);
    service_status->latency = s->latency;
    service_status->max_check_attempts = s->max_attempts;
    service_status->next_check = s->next_check;
    service_status->next_notification = s->next_notification;
    service_status->no_more_notifications = s->no_more_notifications;
    service_status->notifications_enabled = s->notifications_enabled;
    service_status->obsess_over = s->obsess_over_service;
    if (s->plugin_output) {
      service_status->output = s->plugin_output;
      service_status->output.append("\n");
    }
    if (s->long_plugin_output)
      service_status->output.append(s->long_plugin_output);
    service_status->passive_checks_enabled
      = s->accept_passive_service_checks;
    service_status->percent_state_change = s->percent_state_change;
    if (s->perf_data)
      service_status->perf_data = s->perf_data;
    service_status->retry_interval = s->retry_interval;
    if (!s->host_name)
      throw (exceptions::msg() << "unnamed host");
    if (!s->description)
      throw (exceptions::msg() << "unnamed service");
    service_status->host_name = s->host_name;
    service_status->service_description = s->description;
    {
      service_status->host_id = engine::get_host_id(s->host_name);
      service_status->service_id = engine::get_service_id(
                                     s->host_name,
                                     s->description);
      if (!service_status->host_id || !service_status->service_id)
        throw (exceptions::msg() << "could not find ID of service ('"
               << s->host_name << "', '" << s->description << "')");
    }
    service_status->should_be_scheduled = s->should_be_scheduled;
    service_status->state_type = (s->has_been_checked
                                  ? s->state_type
                                  : HARD_STATE);

    // Send event(s).
    gl_publisher.write(service_status);
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating service status event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
}

/**
 *  Unregister callbacks.
 */
void neb::unregister_callbacks() {
  gl_registered_callbacks.clear();
  return ;
}
