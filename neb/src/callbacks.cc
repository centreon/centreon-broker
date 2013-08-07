/*
** Copyright 2009-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/comments.hh"
#include "com/centreon/engine/nebcallbacks.hh"
#include "com/centreon/engine/nebstructs.hh"

using namespace com::centreon::broker;

// Acknowledgement list.
std::map<std::pair<unsigned int, unsigned int>, neb::acknowledgement>
  neb::gl_acknowledgements;

// Downtime list.
struct   private_downtime_params {
  time_t deletion_time;
  time_t end_time;
  bool   started;
  time_t start_time;
};
// Unstarted downtimes.
static umap<unsigned int, private_downtime_params> downtimes;

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
  { NEBCALLBACK_ACKNOWLEDGEMENT_DATA, &neb::callback_acknowledgement },
  { NEBCALLBACK_COMMENT_DATA, &neb::callback_comment },
  { NEBCALLBACK_DOWNTIME_DATA, &neb::callback_downtime },
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

// External function to get program version.
extern "C" {
  char const* get_program_version();
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
    misc::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);

    // Fill output var.
    ack_data = static_cast<nebstruct_acknowledgement_data*>(data);
    ack->acknowledgement_type = ack_data->acknowledgement_type;
    if (ack_data->author_name)
      ack->author = ack_data->author_name;
    if (ack_data->comment_data)
      ack->comment = ack_data->comment_data;
    ack->entry_time = time(NULL);
    if (!ack_data->host_name)
      throw (exceptions::msg() << "unnamed host");
    umap<std::string, int>::const_iterator it1;
    it1 = gl_hosts.find(ack_data->host_name);
    if (it1 == gl_hosts.end())
      throw (exceptions::msg() << "could not find ID of host '"
             << ack_data->host_name << "'");
    ack->host_id = it1->second;
    if (ack_data->service_description) {
      std::map<std::pair<std::string, std::string>,
             std::pair<int, int> >::const_iterator it2;
      it2 = gl_services.find(std::make_pair(
                                    ack_data->host_name,
                                    ack_data->service_description));
      if (it2 == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << ack_data->host_name << "', '"
               << ack_data->service_description << "')");
      ack->service_id = it2->second.second;
    }
    ack->instance_id = instance_id;
    ack->is_sticky = ack_data->is_sticky;
    ack->notify_contacts = ack_data->notify_contacts;
    ack->persistent_comment = ack_data->persistent_comment;
    ack->state = ack_data->state;
    gl_acknowledgements[std::make_pair(ack->host_id, ack->service_id)]
      = *ack;

    // Send event.
    gl_publisher.write(ack.staticCast<io::data>());
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating acknowledgement event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating comment event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_comment_data const* comment_data;
    misc::shared_ptr<neb::comment> comment(new neb::comment);

    // Fill output var.
    comment_data = static_cast<nebstruct_comment_data*>(data);
    if (comment_data->author_name)
      comment->author = comment_data->author_name;
    if (comment_data->comment_data)
      comment->data = comment_data->comment_data;
    comment->comment_type = comment_data->type;
    if (NEBTYPE_COMMENT_DELETE == comment_data->type)
      comment->deletion_time = time(NULL);
    comment->entry_time = comment_data->entry_time;
    comment->entry_type = comment_data->entry_type;
    comment->expire_time = comment_data->expire_time;
    comment->expires = comment_data->expires;
    if (!comment_data->host_name)
      throw (exceptions::msg() << "unnamed host");
    umap<std::string, int>::const_iterator it1;
    it1 = gl_hosts.find(comment_data->host_name);
    if (it1 == gl_hosts.end())
      throw (exceptions::msg() << "could not find ID of host '"
             << comment_data->host_name << "'");
    comment->host_id = it1->second;
    if (comment_data->service_description) {
      std::map<std::pair<std::string, std::string>,
             std::pair<int, int> >::const_iterator it2;
      it2 = gl_services.find(std::make_pair(
                                    comment_data->host_name,
                                    comment_data->service_description));
      if (it2 == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << comment_data->host_name << "', '"
               << comment_data->service_description << "')");
      comment->service_id = it2->second.second;
    }
    comment->instance_id = instance_id;
    comment->internal_id = comment_data->comment_id;
    comment->persistent = comment_data->persistent;
    comment->source = comment_data->source;

    // Send event.
    gl_publisher.write(comment.staticCast<io::data>());
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating comment event: " << e.what();
  }
  // Avoid exception propagation in C code.
  catch (...) {}
  return (0);
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
          // Special HOST_ID custom variable.
          if (!strcmp(cvar->var_name, "HOST_ID")) {
            // Host ID.
            int host_id(strtol(cvar->var_value, NULL, 0));
            if (host_id) {
              // Record host ID.
              neb::gl_hosts[hst->name] = host_id;

              // Generate host event.
              nebstruct_adaptive_host_data nsahd;
              memset(&nsahd, 0, sizeof(nsahd));
              nsahd.type = NEBTYPE_HOST_ADD;
              nsahd.command_type = CMD_NONE;
              nsahd.modified_attribute = MODATTR_ALL;
              nsahd.modified_attributes = MODATTR_ALL;
              nsahd.object_ptr = hst;

              // Callback.
              callback_host(NEBCALLBACK_ADAPTIVE_HOST_DATA, &nsahd);
            }
          }
          // Normal custom variable.
          else {
            // Fill custom variable event.
            umap<std::string, int>::iterator
              it(neb::gl_hosts.find(hst->name));
            if (it != neb::gl_hosts.end()) {
              misc::shared_ptr<custom_variable>
                new_cvar(new custom_variable);
              new_cvar->host_id = it->second;
              new_cvar->modified = false;
              new_cvar->name = cvar->var_name;
              new_cvar->var_type = 0;
              new_cvar->update_time = cvar->timestamp.tv_sec;
              new_cvar->value = cvar->var_value;

              // Send custom variable event.
              logging::info(logging::low)
                << "callbacks: new custom variable '" << new_cvar->name
                << "' on host " << new_cvar->host_id;
              neb::gl_publisher.write(new_cvar.staticCast<io::data>());
            }
          }
        }
      }
      // Service custom variable.
      else if (NEBTYPE_SERVICECUSTOMVARIABLE_ADD == cvar->type) {
        ::service* svc(static_cast< ::service*>(cvar->object_ptr));
        if (svc && svc->description && svc->host_name) {
          // Special SERVICE_ID custom variable.
          if (!strcmp(cvar->var_name, "SERVICE_ID")) {
            // Host ID.
            int host_id;
            {
              umap<std::string, int>::iterator
                it(neb::gl_hosts.find(svc->host_name));
              if (it != neb::gl_hosts.end())
                host_id = it->second;
              else
                host_id = 0;
            }

            // Service ID.
            int service_id(strtol(cvar->var_value, NULL, 0));

            if (host_id && service_id) {
              // Record host ID/service ID.
              neb::gl_services[std::make_pair<std::string, std::string>(svc->host_name, svc->description)]
                = std::make_pair(host_id, service_id);

              // Generate service event.
              nebstruct_adaptive_service_data nsasd;
              memset(&nsasd, 0, sizeof(nsasd));
              nsasd.type = NEBTYPE_SERVICE_ADD;
              nsasd.command_type = CMD_NONE;
              nsasd.modified_attribute = MODATTR_ALL;
              nsasd.modified_attributes = MODATTR_ALL;
              nsasd.object_ptr = svc;

              // Callback.
              callback_service(NEBCALLBACK_ADAPTIVE_SERVICE_DATA, &nsasd);
            }
          }
          // Normal custom variable (discard HOST_ID).
          else if (strcmp(cvar->var_name, "HOST_ID")) {
            // Fill custom variable event.
            std::map<std::pair<std::string, std::string>, std::pair<int, int> >::iterator
              it(neb::gl_services.find(std::make_pair<std::string, std::string>(svc->host_name, svc->description)));
            if (it != neb::gl_services.end()) {
              misc::shared_ptr<custom_variable>
                new_cvar(new custom_variable);
              new_cvar->host_id = it->second.first;
              new_cvar->modified = false;
              new_cvar->name = cvar->var_name;
              new_cvar->service_id = it->second.second;
              new_cvar->var_type = 1;
              new_cvar->update_time = cvar->timestamp.tv_sec;
              new_cvar->value = cvar->var_value;

              // Send custom variable event.
              logging::info(logging::low)
                << "callbacks: new custom variable '" << new_cvar->name
                << "' on service (" << new_cvar->host_id << ", "
                << new_cvar->service_id << ")";
              neb::gl_publisher.write(new_cvar.staticCast<io::data>());
            }
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
        umap<std::string, int>::iterator it;
        it = neb::gl_hosts.find(dep->host_name);
        if (it != neb::gl_hosts.end())
          host_id = it->second;
        else
          host_id = 0;
      }
      else {
        logging::error(logging::medium)
          << "callbacks: dependency callback called without "
          << "valid host";
        host_id = 0;
      }
      if (dep->dependent_host_name) {
        umap<std::string, int>::iterator it;
        it = neb::gl_hosts.find(dep->dependent_host_name);
        if (it != neb::gl_hosts.end())
          dep_host_id = it->second;
        else
          dep_host_id = 0;
      }
      else {
        logging::error(logging::medium)
          << "callbacks: dependency callback called without "
          << "valid dependent host";
        dep_host_id = 0;
      }

      // Generate service dependency event.
      misc::shared_ptr<service_dependency>
        hst_dep(new service_dependency);
      hst_dep->host_id = host_id;
      hst_dep->dependent_host_id = dep_host_id;
      hst_dep->enabled = (nsadd->type != NEBTYPE_HOSTDEPENDENCY_DELETE);
      if (dep->dependency_period)
        hst_dep->dependency_period = dep->dependency_period;
      // XXX
      // if (dep->execution_failure_options)
      //   hst_dep->execution_failure_options
      //     = dep->execution_failure_options;
      hst_dep->inherits_parent = dep->inherits_parent;
      // if (dep->notification_failure_options)
      //   hst_dep->notification_failure_options
      //     = dep->notification_failure_options;
      logging::info(logging::low) << "callbacks: host " << dep_host_id
        << " depends on host " << host_id;

      // Publish dependency event.
      neb::gl_publisher.write(hst_dep.staticCast<io::data>());
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
        std::map<std::pair<std::string, std::string>,
                 std::pair<int, int> >::iterator it;
        it = neb::gl_services.find(
                    std::make_pair<std::string, std::string>(
                           dep->host_name,
                           dep->service_description));
        if (it != neb::gl_services.end()) {
          host_id = it->second.first;
          service_id = it->second.second;
        }
        else {
          host_id = 0;
          service_id = 0;
        }
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
        std::map<std::pair<std::string, std::string>,
                 std::pair<int, int> >::iterator it;
        it = neb::gl_services.find(
                    std::make_pair<std::string, std::string>(
                           dep->dependent_host_name,
                           dep->dependent_service_description));
        if (it != neb::gl_services.end()) {
          dep_host_id = it->second.first;
          dep_service_id = it->second.second;
        }
        else {
          dep_host_id = 0;
          dep_service_id = 0;
        }
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
      // XXX
      // if (dep->execution_failure_options)
      //   svc_dep->execution_failure_options
      //     = dep->execution_failure_options;
      svc_dep->inherits_parent = dep->inherits_parent;
      // if (dep->notification_failure_options)
      //   svc_dep->notification_failure_options
      //     = dep->notification_failure_options;
      logging::info(logging::low) << "callbacks: service ("
        << dep_host_id << ", " << dep_service_id
        << ") depends on service (" << host_id << ", " << service_id
        << ")";

      // Publish dependency event.
      neb::gl_publisher.write(svc_dep.staticCast<io::data>());
    }
  }
  // Avoid exception propagation to C code.
  catch (...) {}

  return (0);
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
  logging::info(logging::medium)
    << "callbacks: generating downtime event";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_downtime_data const* downtime_data;
    misc::shared_ptr<neb::downtime> downtime(new neb::downtime);

    // Fill output var.
    downtime_data = static_cast<nebstruct_downtime_data*>(data);
    if (downtime_data->author_name)
      downtime->author = downtime_data->author_name;
    if (downtime_data->comment_data)
      downtime->comment = downtime_data->comment_data;
    downtime->downtime_type = downtime_data->downtime_type;
    downtime->duration = downtime_data->duration;
    downtime->end_time = downtime_data->end_time;
    downtime->entry_time = downtime_data->entry_time;
    downtime->fixed = downtime_data->fixed;
    if (!downtime_data->host_name)
      throw (exceptions::msg() << "unnamed host");
    umap<std::string, int>::const_iterator it1;
    it1 = gl_hosts.find(downtime_data->host_name);
    if (it1 == gl_hosts.end())
      throw (exceptions::msg() << "could not find ID of host '"
             << downtime_data->host_name << "'");
    downtime->host_id = it1->second;
    if (downtime_data->service_description) {
      std::map<std::pair<std::string, std::string>,
             std::pair<int, int> >::const_iterator it2;
      it2 = gl_services.find(std::make_pair(
              downtime_data->host_name,
              downtime_data->service_description));
      if (it2 == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << downtime_data->host_name << "', '"
               << downtime_data->service_description << "')");
      downtime->service_id = it2->second.second;
    }
    downtime->instance_id = instance_id;
    downtime->internal_id = downtime_data->downtime_id;
    downtime->start_time = downtime_data->start_time;
    downtime->triggered_by = downtime_data->triggered_by;
    private_downtime_params& params(downtimes[downtime->internal_id]);
    if ((NEBTYPE_DOWNTIME_ADD == downtime_data->type)
        || (NEBTYPE_DOWNTIME_LOAD == downtime_data->type)) {
      params.deletion_time = 0;
      params.end_time = 0;
      params.started = false;
      params.start_time = 0;
    }
    else if (NEBTYPE_DOWNTIME_START == downtime_data->type) {
      params.started = true;
      params.start_time = downtime_data->timestamp.tv_sec;
    }
    else if (NEBTYPE_DOWNTIME_STOP == downtime_data->type) {
      if (NEBATTR_DOWNTIME_STOP_CANCELLED == downtime_data->attr)
        params.deletion_time = downtime_data->timestamp.tv_sec;
      params.end_time = downtime_data->timestamp.tv_sec;
    }
    downtime->actual_start_time = params.start_time;
    downtime->actual_end_time = params.end_time;
    downtime->deletion_time = params.deletion_time;
    downtime->was_cancelled = (downtime->deletion_time != 0);
    downtime->was_started = params.started;
    if (NEBTYPE_DOWNTIME_DELETE == downtime_data->type)
      downtimes.erase(downtime->internal_id);

    // Send event.
    gl_publisher.write(downtime.staticCast<io::data>());
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      "generating downtime event: " << e.what();
  }
  // Avoid exception propagation in C code.
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
    misc::shared_ptr<neb::event_handler> event_handler(new neb::event_handler);

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
    umap<std::string, int>::const_iterator it1;
    it1 = gl_hosts.find(event_handler_data->host_name);
    if (it1 == gl_hosts.end())
      throw (exceptions::msg() << "could not find ID of host '"
             << event_handler_data->host_name << "'");
    event_handler->host_id = it1->second;
    if (event_handler_data->service_description) {
      std::map<std::pair<std::string, std::string>,
               std::pair<int, int> >::const_iterator it2;
      it2 = gl_services.find(std::make_pair(
              event_handler_data->host_name,
              event_handler_data->service_description));
      if (it2 == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << event_handler_data->host_name << "', '"
               << event_handler_data->service_description << "')");
      event_handler->service_id = it2->second.second;
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
    gl_publisher.write(event_handler.staticCast<io::data>());
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
            umap<std::string, int>::const_iterator id;
            id = gl_hosts.find(host.toStdString());
            if (id != gl_hosts.end()) {
              // Fill custom variable.
              misc::shared_ptr<neb::custom_variable_status>
                cvs(new neb::custom_variable_status);
              cvs->host_id = id->second;
              cvs->modified = true;
              cvs->name = var_name;
              cvs->service_id = 0;
              cvs->update_time = necd->timestamp.tv_sec;
              cvs->value = var_value;

              // Send event.
              gl_publisher.write(cvs.staticCast<io::data>());
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
            std::map<std::pair<std::string, std::string>,
                   std::pair<int, int> >::const_iterator ids;
            ids = gl_services.find(std::make_pair(host.toStdString(),
                                                  service.toStdString()));
            if (ids != gl_services.end()) {
              // Fill custom variable.
              misc::shared_ptr<neb::custom_variable_status> cvs(
                new neb::custom_variable_status);
              cvs->host_id = ids->second.first;
              cvs->modified = true;
              cvs->name = var_name;
              cvs->service_id = ids->second.second;
              cvs->update_time = necd->timestamp.tv_sec;
              cvs->value = var_value;

              // Send event.
              gl_publisher.write(cvs.staticCast<io::data>());
            }
          }
        }
      }
      else if ((necd->command_type == CMD_DEL_HOST_DOWNTIME)
               || (necd->command_type == CMD_DEL_SVC_DOWNTIME)) {
        unsigned int downtime_id(necd->command_args
                                 ? strtoul(necd->command_args, NULL, 0)
                                 : 0);
        umap<unsigned int, private_downtime_params>::iterator
          it(downtimes.find(downtime_id));
        if (it != downtimes.end())
          it->second.deletion_time = necd->timestamp.tv_sec;
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
    umap<std::string, int>::const_iterator it1;
    it1 = gl_hosts.find(flapping_data->host_name);
    if (it1 == gl_hosts.end())
      throw (exceptions::msg() << "could not find ID of host '"
             << flapping_data->host_name << "'");
    flapping_status->host_id = it1->second;
    if (flapping_data->service_description) {
      std::map<std::pair<std::string, std::string>,
             std::pair<int, int> >::const_iterator it2;
      it2 = gl_services.find(std::make_pair(
              flapping_data->host_name,
              flapping_data->service_description));
      if (it2 == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << flapping_data->host_name << "', '"
               << flapping_data->service_description << "')");
      flapping_status->service_id = it2->second.second;

      // Set comment time.
      ::comment* com = find_service_comment(flapping_data->comment_id);
      if (com)
        flapping_status->comment_time = com->entry_time;
    }
    else {
      ::comment* com = find_host_comment(flapping_data->comment_id);
      if (com)
        flapping_status->comment_time = com->entry_time;
    }
    flapping_status->internal_comment_id = flapping_data->comment_id;
    flapping_status->low_threshold = flapping_data->low_threshold;
    flapping_status->percent_state_change = flapping_data->percent_change;
    // flapping_status->reason_type = XXX;
    flapping_status->flapping_type = flapping_data->flapping_type;

    // Send event.
    gl_publisher.write(flapping_status.staticCast<io::data>());
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
        if (host_group->action_url)
          new_hg->action_url = host_group->action_url;
        if (host_group->alias)
          new_hg->alias = host_group->alias;
        new_hg->enabled
          = (group_data->type != NEBTYPE_HOSTGROUP_DELETE);
        new_hg->instance_id = neb::instance_id;
        new_hg->name = host_group->group_name;
        if (host_group->notes)
          new_hg->notes = host_group->notes;
        if (host_group->notes_url)
          new_hg->notes_url = host_group->notes_url;

        // Send host group event.
        logging::info(logging::low) << "callbacks: new host group '"
          << new_hg->name << " (instance " << new_hg->instance_id
          << ")";
        neb::gl_publisher.write(new_hg.staticCast<io::data>());
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
        if (service_group->action_url)
          new_sg->action_url = service_group->action_url;
        if (service_group->alias)
          new_sg->alias = service_group->alias;
        new_sg->enabled
          = (group_data->type != NEBTYPE_SERVICEGROUP_DELETE);
        new_sg->instance_id = neb::instance_id;
        new_sg->name = service_group->group_name;
        if (service_group->notes)
          new_sg->notes = service_group->notes;
        if (service_group->notes_url)
          new_sg->notes_url = service_group->notes_url;

        // Send service group event.
        logging::info(logging::low) << "callbacks:: new service group '"
          << new_sg->name << " (instance " << new_sg->instance_id
          << ")";
        neb::gl_publisher.write(new_sg.staticCast<io::data>());
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
    if (member_data->type == NEBTYPE_HOSTGROUPMEMBER_ADD) {
      ::host const*
        hst(static_cast< ::host*>(member_data->object_ptr));
      ::hostgroup const*
        hg(static_cast< ::hostgroup*>(member_data->group_ptr));
      if (hst->name && hg->group_name) {
        // Output variable.
        misc::shared_ptr<neb::host_group_member>
          hgm(new neb::host_group_member);
        hgm->group = hg->group_name;
        hgm->instance_id = neb::instance_id;
        umap<std::string, int>::const_iterator it;
        it = neb::gl_hosts.find(hst->name);
        if (it != neb::gl_hosts.end())
          hgm->host_id = it->second;

        // Send host group member event.
        if (hgm->host_id) {
          logging::info(logging::low) << "callbacks: host "
            << hgm->host_id << " is a member of group '" << hgm->group
            << "' on instance " << hgm->instance_id;
          neb::gl_publisher.write(hgm.staticCast<io::data>());
        }
      }
    }
    // Service group member.
    else if (member_data->type == NEBTYPE_SERVICEGROUPMEMBER_ADD) {
      ::service const*
        svc(static_cast< ::service*>(member_data->object_ptr));
      ::servicegroup const*
        sg(static_cast< ::servicegroup*>(member_data->group_ptr));
      if (svc->description
          && sg->group_name
          && svc->host_ptr
          && svc->host_ptr->name) {
        // Output variable.
        misc::shared_ptr<neb::service_group_member>
          sgm(new neb::service_group_member);
        sgm->group = sg->group_name;
        sgm->instance_id = neb::instance_id;
        std::map<std::pair<std::string, std::string>, std::pair<int, int> >::iterator it;
        it = neb::gl_services.find(std::make_pair<std::string, std::string>(
                                     svc->host_ptr->name,
                                     svc->description));
        if (it != neb::gl_services.end()) {
          sgm->host_id = it->second.first;
          sgm->service_id = it->second.second;
        }

        // Send service group member event.
        if (sgm->host_id && sgm->service_id) {
          logging::info(logging::low) << "callbacks: service ("
            << sgm->host_id << ", " << sgm->service_id
            << ") is a member of group '" << sgm->group
            << "' on instance " << sgm->instance_id;
          neb::gl_publisher.write(sgm.staticCast<io::data>());
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
    my_host->current_notification_number
      = h->current_notification_number;
    my_host->current_state = (h->has_been_checked
                              ? h->current_state
                              : 4); // Pending state.
    my_host->default_active_checks_enabled = h->checks_enabled;
    my_host->default_event_handler_enabled = h->event_handler_enabled;
    my_host->default_failure_prediction = h->failure_prediction_enabled;
    my_host->default_flap_detection_enabled = h->flap_detection_enabled;
    my_host->default_notifications_enabled = h->notifications_enabled;
    my_host->default_passive_checks_enabled
      = h->accept_passive_host_checks;
    my_host->default_process_perf_data = h->process_performance_data;
    if (h->display_name)
      my_host->display_name = h->display_name;
    my_host->enabled = (host_data->type != NEBTYPE_HOST_DELETE);
    if (h->event_handler)
      my_host->event_handler = h->event_handler;
    my_host->event_handler_enabled = h->event_handler_enabled;
    my_host->execution_time = h->execution_time;
    my_host->failure_prediction_enabled = h->failure_prediction_enabled;
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
    my_host->instance_id = neb::instance_id;
    my_host->is_flapping = h->is_flapping;
    my_host->last_check = h->last_check;
    my_host->last_hard_state = h->last_hard_state;
    my_host->last_hard_state_change = h->last_hard_state_change;
    my_host->last_notification = h->last_host_notification;
    my_host->last_state_change = h->last_state_change;
    my_host->last_time_down = h->last_time_down;
    my_host->last_time_unreachable = h->last_time_unreachable;
    my_host->last_time_up = h->last_time_up;
    my_host->last_update = h->last_state_history_update;
    my_host->latency = h->latency;
    my_host->low_flap_threshold = h->low_flap_threshold;
    my_host->max_check_attempts = h->max_attempts;
    my_host->modified_attributes = h->modified_attributes;
    my_host->next_check = h->next_check;
    my_host->next_notification = h->next_host_notification;
    my_host->no_more_notifications = h->no_more_notifications;
    if (h->notes)
      my_host->notes = h->notes;
    if (h->notes_url)
      my_host->notes_url = h->notes_url;
    my_host->notification_interval = h->notification_interval;
    if (h->notification_period)
      my_host->notification_period = h->notification_period;
    my_host->notifications_enabled = h->notifications_enabled;
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
    my_host->problem_has_been_acknowledged
      = h->problem_has_been_acknowledged;
    my_host->process_performance_data = h->process_performance_data;
    my_host->retain_nonstatus_information
      = h->retain_nonstatus_information;
    my_host->retain_status_information = h->retain_status_information;
    my_host->retry_interval = h->retry_interval;
    my_host->scheduled_downtime_depth = h->scheduled_downtime_depth;
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
    umap<std::string, int>::iterator
      it(neb::gl_hosts.find(my_host->host_name.toStdString()));
    if (it != neb::gl_hosts.end()) {
      my_host->host_id = it->second;

      // Send host event.
      logging::info(logging::low) << "callbacks:  new host "
        << my_host->host_id << " ('" << my_host->host_name
        << "') on instance " << my_host->instance_id;
      neb::gl_publisher.write(my_host.staticCast<io::data>());

      // Generate existing custom variables.
      for (customvariablesmember* cvar(h->custom_variables);
           cvar;
           cvar = cvar->next)
        if (cvar->variable_name
            && strcmp(cvar->variable_name, "HOST_ID")) {
          nebstruct_custom_variable_data data;
          memset(&data, 0, sizeof(data));
          data.type = NEBTYPE_HOSTCUSTOMVARIABLE_ADD;
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
      umap<std::string, int>::const_iterator it;
      it = gl_hosts.find(hcdata->host_name);
      if (it == gl_hosts.end())
        throw (exceptions::msg() << "could not find ID of host '"
               << hcdata->host_name << "'");
      host_check->host_id = it->second;
      host_check->next_check = h->next_check;

      // Send event.
      gl_publisher.write(host_check.staticCast<io::data>());
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
    host_status->acknowledgement_type = h->acknowledgement_type;
    host_status->active_checks_enabled = h->checks_enabled;
    if (h->host_check_command)
      host_status->check_command = h->host_check_command;
    host_status->check_interval = h->check_interval;
    if (h->check_period)
      host_status->check_period = h->check_period;
    host_status->check_type = h->check_type;
    host_status->current_check_attempt = h->current_attempt;
    host_status->current_notification_number
      = h->current_notification_number;
    host_status->current_state = (h->has_been_checked
                                  ? h->current_state
                                  : 4); // Pending state.
    if (h->event_handler)
      host_status->event_handler = h->event_handler;
    host_status->event_handler_enabled = h->event_handler_enabled;
    host_status->execution_time = h->execution_time;
    host_status->failure_prediction_enabled = h->failure_prediction_enabled;
    host_status->flap_detection_enabled = h->flap_detection_enabled;
    host_status->has_been_checked = h->has_been_checked;
    if (!h->name)
      throw (exceptions::msg() << "unnamed host");
    {
      umap<std::string, int>::const_iterator it;
      it = gl_hosts.find(h->name);
      if (it == gl_hosts.end())
        throw (exceptions::msg() << "could not find ID of host '"
               << h->name << "'");
      host_status->host_id = it->second;
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
    host_status->modified_attributes = h->modified_attributes;
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
    host_status->problem_has_been_acknowledged
      = h->problem_has_been_acknowledged;
    host_status->process_performance_data = h->process_performance_data;
    host_status->retry_interval = h->retry_interval;
    host_status->scheduled_downtime_depth = h->scheduled_downtime_depth;
    host_status->should_be_scheduled = h->should_be_scheduled;
    host_status->state_type = (h->has_been_checked
                               ? h->state_type
                               : HARD_STATE);

    // Send event(s).
    gl_publisher.write(host_status.staticCast<io::data>());
    // Acknowledgement event.
    std::map<
      std::pair<unsigned int, unsigned int>,
      neb::acknowledgement>::iterator
      it(gl_acknowledgements.find(
           std::make_pair(host_status->host_id, 0u)));
    if ((it != gl_acknowledgements.end())
        && !host_status->problem_has_been_acknowledged) {
      if (!(!host_status->current_state // !(OK or (normal ack and NOK))
            || (!it->second.is_sticky
                && (host_status->current_state != it->second.state)))) {
        misc::shared_ptr<neb::acknowledgement>
          ack(new neb::acknowledgement(it->second));
        ack->deletion_time = time(NULL);
        gl_publisher.write(ack.staticCast<io::data>());
      }
      gl_acknowledgements.erase(it);
    }
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
    le->instance_name = instance_name;
    if (log_data->data) {
      if (log_data->data)
        le->output = log_data->data;
      set_log_data(*le, log_data->data);
    }

    // Send event.
    gl_publisher.write(le.staticCast<io::data>());
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
  logging::debug(logging::low) << "callbacks: module event callback";
  (void)callback_type;

  try {
    // In/Out variables.
    nebstruct_module_data const* module_data;
    misc::shared_ptr<neb::module> me(new neb::module);

    // Fill output var.
    module_data = static_cast<nebstruct_module_data*>(data);
    if (module_data->module) {
      me->filename = module_data->module;
      if (module_data->args)
        me->args = module_data->args;
      me->instance_id = instance_id;
      me->loaded = !(module_data->type == NEBTYPE_MODULE_DELETE);
      me->should_be_loaded = true;

      // Send events.
      gl_publisher.write(me.staticCast<io::data>());
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

      // Output variable.
      misc::shared_ptr<neb::instance> instance(new neb::instance);

      // Parse configuration file.
      try {
        config::parser parsr;
        config::state conf;
        parsr.parse(gl_configuration_file, conf);

        // Apply resulting configuration.
        config::applier::state::instance().apply(conf);

        // Set variables.
        instance_id = conf.instance_id();
        instance_name = conf.instance_name();
      }
      catch (exceptions::msg const& e) {
        logging::config(logging::high) << e.what();
        return (0);
      }

      if (neb::gl_mod_flags & NEBMODULE_ENGINE)
        instance->engine = "Centreon Engine";
      else
        instance->engine = "Nagios / Centreon Engine (< 1.3.0)";
      instance->id = instance_id;
      instance->is_running = true;
      instance->name = instance_name;
      instance->pid = getpid();
      instance->program_start = time(NULL);
      instance->version = get_program_version();
      start_time = instance->program_start;

      // Send initial event and then configuration.
      gl_publisher.write(instance.staticCast<io::data>());
      send_initial_configuration();
    }
    else if (NEBTYPE_PROCESS_EVENTLOOPEND == process_data->type) {
      logging::info(logging::medium)
        << "callbacks: generating process end event";
      // Output variable.
      misc::shared_ptr<neb::instance> instance(new neb::instance);

      // Fill output var.
      if (neb::gl_mod_flags & NEBMODULE_ENGINE)
        instance->engine = "Centreon Engine";
      else
        instance->engine = "Nagios / Centreon Engine (< 1.3.0)";
      instance->id = instance_id;
      instance->is_running = false;
      instance->name = instance_name;
      instance->pid = getpid();
      instance->program_end = time(NULL);
      instance->program_start = start_time;
      instance->version = get_program_version();

      // Send event.
      gl_publisher.write(instance.staticCast<io::data>());
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
    is->active_host_checks_enabled
      = program_status_data->active_host_checks_enabled;
    is->active_service_checks_enabled
      = program_status_data->active_service_checks_enabled;
    is->daemon_mode = program_status_data->daemon_mode;
    is->event_handler_enabled
      = program_status_data->event_handlers_enabled;
    is->failure_prediction_enabled
      = program_status_data->failure_prediction_enabled;
    is->flap_detection_enabled
      = program_status_data->flap_detection_enabled;
    if (program_status_data->global_host_event_handler)
      is->global_host_event_handler
        = program_status_data->global_host_event_handler;
    if (program_status_data->global_service_event_handler)
      is->global_service_event_handler
        = program_status_data->global_service_event_handler;
    is->id = instance_id;
    is->last_alive = time(NULL);
    is->last_command_check = program_status_data->last_command_check;
    is->last_log_rotation
      = program_status_data->last_log_rotation;
    is->modified_host_attributes
      = program_status_data->modified_host_attributes;
    is->modified_service_attributes
      = program_status_data->modified_service_attributes;
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
    is->process_performance_data
      = program_status_data->process_performance_data;

    // Send event.
    gl_publisher.write(is.staticCast<io::data>());
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
          umap<std::string, int>::iterator it;
          it = neb::gl_hosts.find(relation->dep_hst->name);
          if (it != neb::gl_hosts.end())
            host_id = it->second;
          else
            host_id = 0;
          it = neb::gl_hosts.find(relation->hst->name);
          if (it != neb::gl_hosts.end())
            parent_id = it->second;
          else
            parent_id = 0;
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
                              new_host_parent.staticCast<io::data>());
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
    my_service->current_notification_number = s->current_notification_number;
    my_service->current_state = (s->has_been_checked
                                 ? s->current_state
                                 : 4); // Pending state.
    my_service->default_active_checks_enabled = s->checks_enabled;
    my_service->default_event_handler_enabled = s->event_handler_enabled;
    my_service->default_failure_prediction = s->failure_prediction_enabled;
    my_service->default_flap_detection_enabled = s->flap_detection_enabled;
    my_service->default_notifications_enabled = s->notifications_enabled;
    my_service->default_passive_checks_enabled
      = s->accept_passive_service_checks;
    my_service->default_process_perf_data
      = s->process_performance_data;
    if (s->display_name)
      my_service->display_name = s->display_name;
    my_service->enabled
      = (service_data->type != NEBTYPE_SERVICE_DELETE);
    if (s->event_handler)
      my_service->event_handler = s->event_handler;
    my_service->event_handler_enabled = s->event_handler_enabled;
    my_service->execution_time = s->execution_time;
    my_service->failure_prediction_enabled = s->failure_prediction_enabled;
    if (s->failure_prediction_options)
      my_service->failure_prediction_options = s->failure_prediction_options;
    my_service->first_notification_delay = s->first_notification_delay;
    my_service->flap_detection_enabled = s->flap_detection_enabled;
    my_service->flap_detection_on_critical = s->flap_detection_on_critical;
    my_service->flap_detection_on_ok = s->flap_detection_on_ok;
    my_service->flap_detection_on_unknown = s->flap_detection_on_unknown;
    my_service->flap_detection_on_warning = s->flap_detection_on_warning;
    my_service->freshness_threshold = s->freshness_threshold;
    my_service->has_been_checked = s->has_been_checked;
    my_service->high_flap_threshold = s->high_flap_threshold;
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
    my_service->last_update = time(NULL); // XXX
    my_service->latency = s->latency;
    my_service->low_flap_threshold = s->low_flap_threshold;
    my_service->max_check_attempts = s->max_attempts;
    my_service->modified_attributes = s->modified_attributes;
    my_service->next_check = s->next_check;
    my_service->next_notification = s->next_notification;
    my_service->no_more_notifications = s->no_more_notifications;
    if (s->notes)
      my_service->notes = s->notes;
    if (s->notes_url)
      my_service->notes_url = s->notes_url;
    my_service->notification_interval = s->notification_interval;
    if (s->notification_period)
      my_service->notification_period = s->notification_period;
    my_service->notifications_enabled = s->notifications_enabled;
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
    my_service->passive_checks_enabled = s->accept_passive_service_checks;
    my_service->percent_state_change = s->percent_state_change;
    if (s->perf_data)
      my_service->perf_data = s->perf_data;
    my_service->problem_has_been_acknowledged = s->problem_has_been_acknowledged;
    my_service->process_performance_data = s->process_performance_data;
    my_service->retain_nonstatus_information
      = s->retain_nonstatus_information;
    my_service->retain_status_information = s->retain_status_information;
    my_service->retry_interval = s->retry_interval;
    my_service->scheduled_downtime_depth = s->scheduled_downtime_depth;
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
    std::map<std::pair<std::string, std::string>, std::pair<int, int> >::iterator
      it(neb::gl_services.find(std::make_pair<std::string, std::string>(
                                 s->host_name ? s->host_name : "",
                                 my_service->service_description.toStdString())));
    if (it != neb::gl_services.end()) {
      my_service->host_id = it->second.first;
      my_service->service_id = it->second.second;

      // Send service event.
      logging::info(logging::low) << "callbacks: new service "
        << my_service->service_id << " ('"
        << my_service->service_description
        << "') on host " << my_service->host_id;
      neb::gl_publisher.write(my_service.staticCast<io::data>());

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
      std::map<std::pair<std::string, std::string>, std::pair<int, int> >::const_iterator it;
      it = gl_services.find(std::make_pair(
                                   scdata->host_name,
                                   scdata->service_description));
      if (it == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << scdata->host_name << "', '"
               << scdata->service_description << "')");
      service_check->host_id = it->second.first;
      service_check->next_check = s->next_check;
      service_check->service_id = it->second.second;

      // Send event.
      gl_publisher.write(service_check.staticCast<io::data>());
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::medium) << "callbacks: error occurred while"
      " generating service check event";
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
    service_status->acknowledgement_type = s->acknowledgement_type;
    service_status->active_checks_enabled = s->checks_enabled;
    if (s->service_check_command)
      service_status->check_command = s->service_check_command;
    service_status->check_interval = s->check_interval;
    if (s->check_period)
      service_status->check_period = s->check_period;
    service_status->check_type = s->check_type;
    service_status->current_check_attempt = s->current_attempt;
    service_status->current_notification_number
      = s->current_notification_number;
    service_status->current_state = (s->has_been_checked
                                     ? s->current_state
                                     : 4); // Pending state.
    if (s->event_handler)
      service_status->event_handler = s->event_handler;
    service_status->event_handler_enabled = s->event_handler_enabled;
    service_status->execution_time = s->execution_time;
    service_status->failure_prediction_enabled
      = s->failure_prediction_enabled;
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
    service_status->modified_attributes = s->modified_attributes;
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
    service_status->problem_has_been_acknowledged
      = s->problem_has_been_acknowledged;
    service_status->process_performance_data = s->process_performance_data;
    service_status->retry_interval = s->retry_interval;
    service_status->scheduled_downtime_depth = s->scheduled_downtime_depth;
    if (!s->host_name)
      throw (exceptions::msg() << "unnamed host");
    if (!s->description)
      throw (exceptions::msg() << "unnamed service");
    service_status->host_name = s->host_name;
    service_status->service_description = s->description;
    {
      std::map<std::pair<std::string, std::string>,
               std::pair<int, int> >::const_iterator it;
      it = gl_services.find(std::make_pair(
                                   s->host_name,
                                   s->description));
      if (it == gl_services.end())
        throw (exceptions::msg() << "could not find ID of service ('"
               << s->host_name << "', '" << s->description << "')");
      service_status->host_id = it->second.first;
      service_status->service_id = it->second.second;
    }
    service_status->should_be_scheduled = s->should_be_scheduled;
    service_status->state_type = (s->has_been_checked
                                  ? s->state_type
                                  : HARD_STATE);

    // Send event(s).
    gl_publisher.write(service_status.staticCast<io::data>());
    // Acknowledgement event.
    std::map<
      std::pair<unsigned int, unsigned int>,
      neb::acknowledgement>::iterator
      it(gl_acknowledgements.find(std::make_pair(
                                         service_status->host_id,
                                         service_status->service_id)));
    if ((it != gl_acknowledgements.end())
        && !service_status->problem_has_been_acknowledged) {
      if (!(!service_status->current_state // !(OK or (normal ack and NOK))
            || (!it->second.is_sticky
                && (service_status->current_state
                    != it->second.state)))) {
        misc::shared_ptr<neb::acknowledgement>
          ack(new neb::acknowledgement(it->second));
        ack->deletion_time = time(NULL);
        gl_publisher.write(ack.staticCast<io::data>());
      }
      gl_acknowledgements.erase(it);
    }
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
