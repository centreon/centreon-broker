/*
** Copyright 2012,2015 Centreon
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

#include <csignal>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/centengine.hh"
#include "test/centengine_config.hh"
#include "test/misc.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] cfg  Pointer to centengine configuration object.
 *                  Configuration object must remain valid
 *                  thorough centengine lifetime.
 */
centengine::centengine(centengine_config const* cfg) : _config(cfg) {
  // Create base directory.
  _config_path = tmpnam(NULL);
  mkdir(_config_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

  // Write configuration files.
  _write_cfg();
}

/**
 *  Destructor.
 */
centengine::~centengine() {
  stop();
  recursive_remove(_config_path);
}

/**
 *  Reload Engine.
 */
void centengine::reload() {
  _write_cfg();
  pid_t pid(_engine.pid());
  if ((pid != (pid_t)0) && (pid != (pid_t)-1))
    kill(pid, SIGHUP);
  return ;
}

/**
 *  Start monitoring engine.
 */
void centengine::start() {
  if (_engine.state() == QProcess::NotRunning) {
    QString config_file;
    config_file = _config_path.c_str();
    config_file.append("/centengine.cfg");
    QStringList args;
    args.push_back(config_file);
    _engine.start(MONITORING_ENGINE, args);
    _engine.waitForStarted();
  }
  return ;
}

/**
 *  Stop monitoring engine.
 */
void centengine::stop() {
  if (_engine.state() != QProcess::NotRunning) {
    _engine.terminate();
    _engine.waitForFinished(20000);
    if (_engine.state() != QProcess::NotRunning) {
      _engine.kill();
      _engine.waitForFinished(-1);
    }
  }
  return ;
}

/**
 *  Write configuration files.
 */
void centengine::_write_cfg() {
  // Open base file.
  std::ofstream ofs;
  {
    std::ostringstream oss;
    oss << _config_path << "/centengine.cfg";
    ofs.open(
          oss.str().c_str(),
          std::ios_base::out | std::ios_base::trunc);
    if (ofs.fail())
      throw (exceptions::msg() << "cannot open configuration file '"
             << _config_path << "'");
  }

  // Write configured directives.
  for (std::map<std::string, std::string>::const_iterator
         it(_config->get_directives().begin()),
         end(_config->get_directives().end());
       it != end;
       ++it)
    ofs << it->first << "=" << it->second << "\n";

  // Subconfiguration files.
  std::string hosts_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/hosts.cfg";
    hosts_file = oss.str();
  }
  std::string services_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/services.cfg";
    services_file = oss.str();
  }
  std::string commands_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/commands.cfg";
    commands_file = oss.str();
  }
  std::string host_dependencies_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/host_dependencies.cfg";
    host_dependencies_file = oss.str();
  }
  std::string service_dependencies_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/service_dependencies.cfg";
    service_dependencies_file = oss.str();
  }
  std::string misc_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/misc.cfg";
    misc_file = oss.str();
  }
  ofs << "cfg_file=" << hosts_file << "\n"
      << "cfg_file=" << services_file << "\n"
      << "cfg_file=" << commands_file << "\n"
      << "cfg_file=" << host_dependencies_file << "\n"
      << "cfg_file=" << service_dependencies_file << "\n"
      << "cfg_file=" << misc_file << "\n";

  // cbmod.
  ofs << "broker_module=" << CBMOD_PATH << " "
      << _config->get_cbmod_cfg_file() << "\n";

  // External command.
  ofs << _extcmd.get_engine_config();

  // Additional configuration.
  ofs << MONITORING_ENGINE_ADDITIONAL;

  // Close base file.
  ofs.close();

  // Hosts.
  ofs.open(
        hosts_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open hosts configuration file in '"
           << _config_path << "'");
  ofs << "define host{\n"
      << "  host_name default_host\n"
      << "  address localhost\n"
      << "  active_checks_enabled 0\n"
    // Deprecated in Centreon Engine 2.x.
      << "  alias default_host\n"
      << "  check_command default_command\n"
      << "  max_check_attempts 3\n"
      << "  check_period default_timeperiod\n"
      << "  contacts default_contact\n"
      << "}\n\n";
  for (std::list<host>::const_iterator
         it(_config->get_hosts().begin()),
         end(_config->get_hosts().end());
       it != end;
       ++it) {
    ofs << "define host{\n"
        << "  host_name " << it->name << "\n"
        << "  alias " << (it->alias ? it->alias : it->name) << "\n"
        << "  address " << (it->address ? it->address : "localhost")
        << "\n"
        << "  active_checks_enabled " << it->checks_enabled << "\n"
        << "  check_command " << (it->host_check_command
                                  ? it->host_check_command
                                  : "default_command") << "\n"
        << "  max_check_attempts " << ((it->max_attempts > 0)
                                       ? it->max_attempts
                                       : 3) << "\n"
        << "  check_interval "
        << ((it->check_interval > 0) ? it->check_interval : 5) << "\n"
        << "  retry_interval "
        << ((it->retry_interval > 0) ? it->retry_interval : 3) << "\n"
        << "  check_period " << (it->check_period
                                 ? it->check_period
                                 : "default_timeperiod") << "\n"
        << "  event_handler_enabled " << it->event_handler_enabled
        << "\n"
      // Deprecated in Centreon Engine 2.x.
        << "  passive_checks_enabled "
        << it->accept_passive_host_checks << "\n"
        << "  notification_interval "
        << ((it->notification_interval > 0)
            ? it->notification_interval
            : 10) << "\n"
        << "  notification_period " << (it->notification_period
                                        ? it->notification_period
                                        : "default_timeperiod")
        << "\n"
        << "  contacts ";
    if (it->contacts) {
      ofs << it->contacts->contact_name;
      for (contactsmember* mbr(it->contacts->next);
           mbr;
           mbr = mbr->next)
        ofs << "," << mbr->contact_name;
    }
    else
      ofs << "default_contact";
    ofs << "\n";
    if (it->parent_hosts) {
      ofs << "  parents " << it->parent_hosts->host_name;
      for (hostsmember* parent(it->parent_hosts->next);
           parent;
           parent = parent->next)
        ofs << "," << parent->host_name;
      ofs << "\n";
    }
    if (it->event_handler)
      ofs << "  event_handler " << it->event_handler << "\n";
    for (customvariablesmember* cvar(it->custom_variables);
         cvar;
         cvar = cvar->next)
      ofs << "  _" << cvar->variable_name << " "
          << cvar->variable_value << "\n";
    ofs << "}\n\n";
  }
  ofs.close();

  // Services.
  ofs.open(
        services_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open services configuration file in '"
           << _config_path << "'");
  ofs << "define service{\n"
      << "  service_description default_service\n"
      << "  host_name default_host\n"
      << "  active_checks_enabled 0\n"
    // Deprecated in Centreon Engine 2.x
      << "  check_command default_command\n"
      << "  max_check_attempts 3\n"
      << "  check_interval 5\n"
      << "  retry_interval 3\n"
      << "  check_period default_timeperiod\n"
      << "  notification_interval 10\n"
      << "  notification_period default_timeperiod\n"
      << "  contacts default_contact\n"
      << "}\n\n";
  for (std::list<service>::const_iterator
         it(_config->get_services().begin()),
         end(_config->get_services().end());
       it != end;
       ++it) {
    ofs << "define service{\n"
        << "  service_description " << it->description << "\n"
        << "  host_name " << it->host_name << "\n"
        << "  active_checks_enabled " << it->checks_enabled << "\n"
        << "  check_command "
        << (it->service_check_command
            ? it->service_check_command
            : "default_command") << "\n"
        << "  max_check_attempts "
        << ((it->max_attempts > 0) ? it->max_attempts : 3) << "\n"
        << "  check_interval "
        << ((it->check_interval > 0) ? it->check_interval : 5) << "\n"
        << "  retry_interval "
        << ((it->retry_interval > 0) ? it->retry_interval : 3) << "\n"
        << "  check_period " << (it->check_period
                                 ? it->check_period
                                 : "default_timeperiod") << "\n"
        << "  event_handler_enabled " << it->event_handler_enabled
        << "\n"
      // Deprecated in Centreon Engine 2.x.
        << "  passive_checks_enabled "
        << it->accept_passive_service_checks << "\n"
        << "  notification_interval "
        << ((it->notification_interval > 0)
            ? it->notification_interval
            : 10) << "\n"
        << "  notification_period "
        << (it->notification_period
            ? it->notification_period
            : "default_timeperiod") << "\n"
        << "  contacts ";
    if (it->contacts) {
      ofs << it->contacts->contact_name;
      for (contactsmember* mbr(it->contacts->next);
           mbr;
           mbr = mbr->next)
        ofs << "," << mbr->contact_name;
    }
    else
      ofs << "default_contact";
    ofs << "\n";
    if (it->event_handler)
      ofs << "  event_handler " << it->event_handler << "\n";
    for (customvariablesmember* cvar(it->custom_variables);
         cvar;
         cvar = cvar->next)
      ofs << "  _" << cvar->variable_name << " "
          << cvar->variable_value << "\n";
    ofs << "}\n\n";
  }
  ofs.close();

  // Commands.
  ofs.open(
        commands_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open commands configuration file in '"
           << _config_path << "'");
  ofs << "define command{\n"
      << "  command_name default_command\n"
      << "  command_line " MY_PLUGIN_PATH " 0\n"
      << "}\n\n";
  for (std::list<command>::const_iterator
         it(_config->get_commands().begin()),
         end(_config->get_commands().end());
       it != end;
       ++it) {
    ofs << "define command{\n"
        << "  command_name " << it->name << "\n"
        << "  command_line "
        << (it->command_line ? it->command_line : MY_PLUGIN_PATH " 0")
        << "\n"
        << "}\n\n";
  }
  ofs.close();

  // Host dependencies.
  ofs.open(
        host_dependencies_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open host dependencies configuration file in '"
           << _config_path << "'");
  for (std::list<hostdependency>::const_iterator
         it(_config->get_host_dependencies().begin()),
         end(_config->get_host_dependencies().end());
       it != end;
       ++it) {
    ofs << "define hostdependency{\n"
        << "  dependent_host_name " << it->dependent_host_name << "\n"
        << "  host_name " << it->host_name << "\n"
      // Is failure_criteria in Centreon Engine 2.x.
        << "  notification_failure_criteria d,u\n"
        << "}\n\n";
  }
  ofs.close();

  // Service dependencies.
  ofs.open(
        service_dependencies_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open service dependencies configuration file in '"
           << _config_path << "'");
  for (std::list<servicedependency>::const_iterator
         it(_config->get_service_dependencies().begin()),
         end(_config->get_service_dependencies().end());
       it != end;
       ++it) {
    ofs << "define servicedependency{\n"
        << "  dependent_host_name " << it->dependent_host_name << "\n"
        << "  dependent_service_description "
        << it->dependent_service_description << "\n"
        << "  host_name " << it->host_name << "\n"
        << "  service_description " << it->service_description << "\n"
      // Is failure_criteria in Centreon Engine 2.x.
        << "  notification_failure_criteria w,c,u\n"
        << "}\n\n";
  }
  ofs.close();

  // Misc.
  ofs.open(
        misc_file.c_str(),
        std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw (exceptions::msg()
           << "cannot open misc configuration file in '"
           << _config_path << "'");
  ofs << "define timeperiod{\n"
      << "  timeperiod_name default_timeperiod\n"
      << "  alias MyTimePeriod\n"
      << "  monday 00:00-24:00\n"
      << "  tuesday 00:00-24:00\n"
      << "  wednesday 00:00-24:00\n"
      << "  thursday 00:00-24:00\n"
      << "  friday 00:00-24:00\n"
      << "  saturday 00:00-24:00\n"
      << "  sunday 00:00-24:00\n"
      << "}\n"
      << "\n"
      << "define contact{\n"
      << "  contact_name default_contact\n"
      << "  host_notification_period default_timeperiod\n"
      << "  host_notification_commands default_command\n"
      << "  service_notification_period default_timeperiod\n"
      << "  service_notification_commands default_command\n"
      << "}\n"
      << "\n";
  ofs.close();
}
