/*
** Copyright 2009-2012 Merethis
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
#include <cstring>
#include <memory>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/callbacks.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/initial.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/engine/broker.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/nebcallbacks.hh"
#include "com/centreon/engine/nebstructs.hh"
#include "com/centreon/engine/objects.hh"

// Internal Nagios host list.
extern "C" {
  extern host* host_list;
  extern hostdependency* hostdependency_list;
  extern hostgroup* hostgroup_list;
  extern service* service_list;
  extern servicedependency* servicedependency_list;
  extern servicegroup* servicegroup_list;
}

using namespace com::centreon::broker;

// NEB module list.
extern "C" {
  nebmodule* neb_module_list;
}

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Send to the global publisher the list of custom variables.
 */
static void send_custom_variables_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning custom variables dump";

  // Iterate through all hosts.
  for (host* h(host_list); h; h = h->next)
    // Send all custom variables.
    for (customvariablesmember* cv(h->custom_variables);
         cv;
         cv = cv->next) {
      // Fill callback struct.
      nebstruct_custom_variable_data nscvd;
      memset(&nscvd, 0, sizeof(nscvd));
      nscvd.type = NEBTYPE_HOSTCUSTOMVARIABLE_ADD;
      nscvd.var_name = cv->variable_name;
      nscvd.var_value = cv->variable_value;
      nscvd.object_ptr = h;

      // Callback.
      neb::callback_custom_variable(
             NEBCALLBACK_CUSTOM_VARIABLE_DATA,
             &nscvd);
    }

  // Iterate through all services.
  for (service* s(service_list); s; s = s->next)
    // Send all custom variables.
    for (customvariablesmember* cv(s->custom_variables);
         cv;
         cv = cv->next) {
      // Fill callback struct.
      nebstruct_custom_variable_data nscvd;
      memset(&nscvd, 0, sizeof(nscvd));
      nscvd.type = NEBTYPE_SERVICECUSTOMVARIABLE_ADD;
      nscvd.var_name = cv->variable_name;
      nscvd.var_value = cv->variable_value;
      nscvd.object_ptr = s;

      // Callback.
      neb::callback_custom_variable(
             NEBCALLBACK_CUSTOM_VARIABLE_DATA,
             &nscvd);
    }

  // End log message.
  logging::info(logging::medium)
    << "init: end of custom variables dump";

  return ;
}

/**
 *  Send to the global publisher the list of host dependencies within Nagios.
 */
static void send_host_dependencies_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning host dependencies dump";

  // Loop through all dependencies.
  for (hostdependency* hd = hostdependency_list; hd; hd = hd->next) {
    misc::shared_ptr<neb::host_dependency> host_dependency(
      new neb::host_dependency);
    std::map<std::string, int>::const_iterator it;

    // Set host dependency parameters.
    if (hd->dependency_period)
      host_dependency->dependency_period = hd->dependency_period;
    if (hd->dependent_host_name) {
      it = neb::gl_hosts.find(hd->dependent_host_name);
      if (it != neb::gl_hosts.end())
        host_dependency->dependent_host_id = it->second;
    }
    host_dependency->inherits_parent = hd->inherits_parent;
    if (hd->host_name) {
      it = neb::gl_hosts.find(hd->host_name);
      if (it != neb::gl_hosts.end())
        host_dependency->host_id = it->second;
    }

    // Send host dependency event.
    logging::info(logging::low) << "init:  host "
      << host_dependency->dependent_host_id << " depends on host "
      << host_dependency->host_id;
    neb::gl_publisher.write(host_dependency.staticCast<io::data>());
  }

  // End log message.
  logging::info(logging::medium)
    << "init: end of host dependencies dump";

  return ;
}

/**
 *  Send to the global publisher the list of host groups within Engine.
 */
static void send_host_group_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning host group dump";

  // Loop through all host groups.
  for (hostgroup* hg(hostgroup_list); hg; hg = hg->next) {
    // Fill callback struct.
    nebstruct_group_data nsgd;
    memset(&nsgd, 0, sizeof(nsgd));
    nsgd.type = NEBTYPE_HOSTGROUP_ADD;
    nsgd.object_ptr = hg;

    // Callback.
    neb::callback_group(NEBCALLBACK_GROUP_DATA, &nsgd);

    // Dump host group members.
    for (hostsmember* hgm(hg->members); hgm; hgm = hgm->next) {
      // Fill callback struct.
      nebstruct_group_member_data nsgmd;
      memset(&nsgmd, 0, sizeof(nsgmd));
      nsgmd.type = NEBTYPE_HOSTGROUPMEMBER_ADD;
      nsgmd.object_ptr = hgm->host_ptr;
      nsgmd.group_ptr = hg;

      // Callback.
      neb::callback_group_member(NEBCALLBACK_GROUP_MEMBER_DATA, &nsgmd);
    }
  }

  // End log message.
  logging::info(logging::medium)
    << "init: end of host group dump";

  return ;
}

/**
 *  Send to the global publisher the list of hosts within Nagios.
 */
static void send_host_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning host dump";

  // Loop through all hosts.
  for (host* h(host_list); h; h = h->next) {
    // Fill callback struct.
    nebstruct_adaptive_host_data nsahd;
    memset(&nsahd, 0, sizeof(nsahd));
    nsahd.type = NEBTYPE_HOST_ADD;
    nsahd.command_type = CMD_NONE;
    nsahd.modified_attribute = MODATTR_ALL;
    nsahd.modified_attributes = MODATTR_ALL;
    nsahd.object_ptr = h;

    // Callback.
    neb::callback_host(NEBCALLBACK_ADAPTIVE_HOST_DATA, &nsahd);
  }

  // End log message.
  logging::info(logging::medium) << "init: end of host dump";

  return ;
}

/**
 *  Send to the global publisher the list of host parents within Nagios.
 */
static void send_host_parents_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning host parents dump";

  // Loop through all hosts.
  int host_id;
  for (host* h = host_list; h; h = h->next) {
    std::map<std::string, int>::const_iterator it;

    // Search host_id.
    if (h->name) {
      it = neb::gl_hosts.find(h->name);
      if (it != neb::gl_hosts.end())
        host_id = it->second;
      else
        host_id = 0;
    }
    else
      host_id = 0;

    // Loop through all dependencies.
    for (hostsmember* parent = h->parent_hosts; parent; parent = parent->next) {
      misc::shared_ptr<neb::host_parent> hp(new neb::host_parent);
      std::map<std::string, int>::const_iterator it;

      hp->host_id = host_id;
      if (parent->host_name) {
        it = neb::gl_hosts.find(parent->host_name);
        if (it != neb::gl_hosts.end())
          hp->parent_id = it->second;
      }

      // Send host parent event.
      logging::info(logging::low) << "init:  host " << hp->parent_id
        << " is parent of host " << hp->host_id;
      neb::gl_publisher.write(hp.staticCast<io::data>());
    }
  }

  // End log message.
  logging::info(logging::medium) << "init: end of host parents dump";

  return ;
}

/**
 *  Send to the global publisher the list of modules loaded by Engine.
 */
static void send_module_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning modules dump";

  // Browse module list.
  for (nebmodule* nm(neb_module_list); nm; nm = nm->next)
    if (nm->filename) {
      // Fill callback struct.
      nebstruct_module_data nsmd;
      memset(&nsmd, 0, sizeof(nsmd));
      nsmd.module = nm->filename;
      nsmd.args = nm->args;
      nsmd.type = NEBTYPE_MODULE_ADD;

      // Callback.
      neb::callback_module(NEBTYPE_MODULE_ADD, &nsmd);
    }

  // End log message.
  logging::info(logging::medium) << "init: end of modules dump";

  return ;
}

/**
 *  Send to the global publisher the list of service dependencies within
 *  Nagios.
 */
static void send_service_dependencies_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning service dependencies dump";

  // Loop through all dependencies.
  for (servicedependency* sd = servicedependency_list; sd; sd = sd->next) {
    std::map<std::pair<std::string, std::string>, std::pair<int, int> >::const_iterator it;
    misc::shared_ptr<neb::service_dependency>
      service_dependency(new neb::service_dependency);

    // Search IDs.
    if (sd->dependent_host_name && sd->dependent_service_description) {
      it = neb::gl_services.find(std::make_pair<std::string, std::string>(
             sd->dependent_host_name, sd->dependent_service_description));
      if (it != neb::gl_services.end()) {
        service_dependency->dependent_host_id = it->second.first;
        service_dependency->dependent_service_id = it->second.second;
      }
    }
    if (sd->dependency_period)
      service_dependency->dependency_period = sd->dependency_period;
    service_dependency->inherits_parent = sd->inherits_parent;
    if (sd->host_name && sd->service_description) {
      it = neb::gl_services.find(std::make_pair<std::string, std::string>(
             sd->host_name, sd->service_description));
      if (it != neb::gl_services.end()) {
        service_dependency->host_id = it->second.first;
        service_dependency->service_id = it->second.second;
      }
    }

    // Send service dependency event.
    logging::info(logging::low) << "init:  service ("
      << service_dependency->dependent_host_id << ", "
      << service_dependency->dependent_service_id
      << ") depends on service (" << service_dependency->host_id
      << ", " << service_dependency->service_id << ")";
    neb::gl_publisher.write(service_dependency.staticCast<io::data>());
  }

  // End log message.
  logging::info(logging::medium)
    << "init: end of service dependencies dump";

  return ;
}

/**
 *  Send to the global publisher the list of service groups within Engine.
 */
static void send_service_group_list() {
  // Start log message.
  logging::info(logging::medium)
    << "init: beginning service group dump";

  // Loop through all service groups.
  for (servicegroup* sg(servicegroup_list); sg; sg = sg->next) {
    // Fill callback struct.
    nebstruct_group_data nsgd;
    memset(&nsgd, 0, sizeof(nsgd));
    nsgd.type = NEBTYPE_SERVICEGROUP_ADD;
    nsgd.object_ptr = sg;

    // Callback.
    neb::callback_group(NEBCALLBACK_GROUP_DATA, &nsgd);

    // Dump service group members.
    for (servicesmember* sgm(sg->members); sgm; sgm = sgm->next) {
      // Fill callback struct.
      nebstruct_group_member_data nsgmd;
      memset(&nsgmd, 0, sizeof(nsgmd));
      nsgmd.type = NEBTYPE_SERVICEGROUPMEMBER_ADD;
      nsgmd.object_ptr = sgm->service_ptr;
      nsgmd.group_ptr = sg;

      // Callback.
      neb::callback_group_member(NEBCALLBACK_GROUP_MEMBER_DATA, &nsgmd);
    }
  }

  // End log message.
  logging::info(logging::medium) << "init: end of service groups dump";

  return ;
}

/**
 *  Send to the global publisher the list of services within Nagios.
 */
static void send_service_list() {
  // Start log message.
  logging::info(logging::medium) << "init: beginning service dump";

  // Loop through all services.
  for (service* s = service_list; s; s = s->next) {
    // Fill callback struct.
    nebstruct_adaptive_service_data nsasd;
    memset(&nsasd, 0, sizeof(nsasd));
    nsasd.type = NEBTYPE_SERVICE_ADD;
    nsasd.command_type = CMD_NONE;
    nsasd.modified_attribute = MODATTR_ALL;
    nsasd.modified_attributes = MODATTR_ALL;
    nsasd.object_ptr = s;

    // Callback.
    neb::callback_service(NEBCALLBACK_ADAPTIVE_SERVICE_DATA, &nsasd);
  }

  // End log message.
  logging::info(logging::medium) << "init: end of services dump";

  return ;
}

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  Send initial configuration to the global publisher.
 */
void neb::send_initial_configuration() {
  send_host_list();
  send_service_list();
  send_custom_variables_list();
  send_host_parents_list();
  send_host_group_list();
  send_service_group_list();
  send_host_dependencies_list();
  send_service_dependencies_list();
  send_module_list();
  return ;
}
