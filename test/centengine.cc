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

#include "test/centengine.hh"
#include <sys/stat.h>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/misc/misc.hh"
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
  _config_path = misc::temp_path();
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
 *  Get external command object.
 *
 *  @return External command object.
 */
centengine_extcmd& centengine::extcmd() {
  return (_extcmd);
}

/**
 *  Reload Engine.
 */
void centengine::reload() {
  _write_cfg();
  pid_t pid(_engine.pid());
  if ((pid != (pid_t)0) && (pid != (pid_t)-1))
    kill(pid, SIGHUP);
  return;
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
  return;
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
  return;
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
    ofs.open(oss.str().c_str(), std::ios_base::out | std::ios_base::trunc);
    if (ofs.fail())
      throw(exceptions::msg()
            << "cannot open configuration file '" << _config_path << "'");
  }

  // Write configured directives.
  for (std::map<std::string, std::string>::const_iterator
           it(_config->get_directives().begin()),
       end(_config->get_directives().end());
       it != end; ++it)
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
  std::string host_groups_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/host_groups.cfg";
    host_groups_file = oss.str();
  }
  std::string service_groups_file;
  {
    std::ostringstream oss;
    oss << _config_path << "/service_groups.cfg";
    service_groups_file = oss.str();
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
      << "cfg_file=" << host_groups_file << "\n"
      << "cfg_file=" << service_groups_file << "\n"
      << "cfg_file=" << misc_file << "\n";

  // cbmod.
  ofs << "broker_module=" << CBMOD_PATH << " " << _config->get_cbmod_cfg_file()
      << "\n";

  // External command.
  ofs << _extcmd.get_engine_config();

  // Additional configuration.
  ofs << MONITORING_ENGINE_ADDITIONAL;

  // Close base file.
  ofs.close();

  // Hosts.
  ofs.open(hosts_file.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg() << "cannot open hosts configuration file in '"
                            << _config_path << "'");
  _write_objs(ofs, _config->get_hosts());
  ofs.close();

  // Services.
  ofs.open(services_file.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg() << "cannot open services configuration file in '"
                            << _config_path << "'");
  _write_objs(ofs, _config->get_services());
  ofs.close();

  // Commands.
  ofs.open(commands_file.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg() << "cannot open commands configuration file in '"
                            << _config_path << "'");
  _write_objs(ofs, _config->get_commands());
  ofs.close();

  // Host dependencies.
  ofs.open(host_dependencies_file.c_str(),
           std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg()
          << "cannot open host dependencies configuration file in '"
          << _config_path << "'");
  _write_objs(ofs, _config->get_host_dependencies());
  ofs.close();

  // Service dependencies.
  ofs.open(service_dependencies_file.c_str(),
           std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg()
          << "cannot open service dependencies configuration file in '"
          << _config_path << "'");
  _write_objs(ofs, _config->get_service_dependencies());
  ofs.close();

  // Host groups.
  ofs.open(host_groups_file.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg() << "cannot open host groups configuration file in '"
                            << _config_path << "'");
  _write_objs(ofs, _config->get_host_groups());
  ofs.close();

  // Service groups.
  ofs.open(service_groups_file.c_str(),
           std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg()
          << "cannot open service groups configuration file in '"
          << _config_path << "'");
  _write_objs(ofs, _config->get_service_groups());
  ofs.close();

  // Misc.
  ofs.open(misc_file.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (ofs.fail())
    throw(exceptions::msg()
          << "cannot open misc configuration file in '" << _config_path << "'");
  _write_objs(ofs, _config->get_contacts());
  _write_objs(ofs, _config->get_timeperiods());
  ofs.close();
}

/**
 *  Write objects to a file.
 *
 *  @param[out] ofs   Output stream.
 *  @param[in]  objs  Objects.
 */
void centengine::_write_objs(std::ofstream& ofs,
                             centengine_config::objlist const& objs) {
  for (centengine_config::objlist::const_iterator it_obj(objs.begin()),
       end_obj(objs.end());
       it_obj != end_obj; ++it_obj) {
    ofs << "define ";
    switch (it_obj->get_type()) {
      case centengine_object::command_type:
        ofs << "command";
        break;
      case centengine_object::contact_type:
        ofs << "contact";
        break;
      case centengine_object::host_type:
        ofs << "host";
        break;
      case centengine_object::hostdependency_type:
        ofs << "hostdependency";
        break;
      case centengine_object::hostgroup_type:
        ofs << "hostgroup";
        break;
      case centengine_object::service_type:
        ofs << "service";
        break;
      case centengine_object::servicedependency_type:
        ofs << "servicedependency";
        break;
      case centengine_object::servicegroup_type:
        ofs << "servicegroup";
        break;
      case centengine_object::timeperiod_type:
        ofs << "timeperiod";
        break;
      default:
        throw(exceptions::msg()
              << "invalid object type " << it_obj->get_type());
    };
    ofs << " {\n";
    for (std::map<std::string, std::string>::const_iterator
             it_prop(it_obj->get_variables().begin()),
         end_prop(it_obj->get_variables().end());
         it_prop != end_prop; ++it_prop)
      ofs << it_prop->first << "  " << it_prop->second << "\n";
    ofs << "}\n";
  }
  return;
}
