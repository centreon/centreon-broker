/*
** Copyright 2011-2013, 2021 Centreon
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

#include "com/centreon/broker/watchdog/configuration_parser.hh"

#include <cstring>
#include <fstream>
#include <streambuf>

#include "com/centreon/broker/vars.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker::watchdog;
using namespace nlohmann;

/**
 *  Default constructor.
 */
configuration_parser::configuration_parser() {}

/**
 *  Destructor.
 */
configuration_parser::~configuration_parser() {}

/**
 *  Parse a file.
 *
 *  @param[in] file  The file to parse.
 *
 *  @return  The configuration.
 */
configuration configuration_parser::parse(std::string const& config_filename) {
  _parse_file(config_filename);
  return configuration(_log_path, _instances_configuration);
}

/**
 *  Parse a file into a xml document.
 *
 *  @param[in] config_filename  The config file name.
 */
void configuration_parser::_parse_file(std::string const& config_filename) {
  // Parse json file
  std::ifstream f(config_filename);

  if (f.fail())
    throw msg_fmt("Config parser: Cannot read file '{}': {}", config_filename,
                  std::strerror(errno));

  std::string const& json_to_parse{std::istreambuf_iterator<char>(f),
                                   std::istreambuf_iterator<char>()};
  std::string err;

  try {
    _json_document = json::parse(json_to_parse);
  } catch (const json::parse_error& e) {
    err = e.what();
  }

  if (_json_document.is_null())
    throw msg_fmt("Config parser: Cannot parse file '{}': {}", config_filename,
                  err);

  if (!_json_document.is_object() ||
      _json_document.find("centreonBroker") == _json_document.end() ||
      !_json_document["centreonBroker"].is_object())
    throw msg_fmt(
        "Config parser: Cannot parse file '{}': it must contain a "
        "'centreonBroker' object",
        config_filename);

  try {
    _check_json_document();
  } catch (const json::parse_error& e) {
    throw msg_fmt(
        "Config parser: Cannot parse file '{}': it contains an error: {}",
        e.what());
  }
}

/**
 *  Parse the xml document.
 */
void configuration_parser::_check_json_document() {
  for (auto it = _json_document["centreonBroker"].begin(),
            end = _json_document["centreonBroker"].end();
       it != end; ++it) {
    if (it.key() == "log")
      _log_path = it.value().get<std::string>();
    else if (it.key() == "cbd") {
      if (it.value().is_array())
        for (auto itt : it.value().items()) {
          _parse_centreon_broker_element(itt.value());
        }
      else if (it.value().is_object())
        _parse_centreon_broker_element(it.value());
      else
        throw msg_fmt("error in watchdog config syntax 'cbd' must be an array");
    } else
      throw msg_fmt("error in watchdog config '{}' key is not recognized",
                    it.key());
  }
}

/**
 *  Parse an xml element containing a centreon broker declaration.
 *
 *  @param[in] element  The element.
 */
void configuration_parser::_parse_centreon_broker_element(const json& element) {
  // The default are sane.
  std::string executable;
  json instance_name;
  json instance_config;
  json run;
  json reload;

  auto it = element.find("executable");
  if (it != element.end())
    executable = it.value();

  it = element.find("name");
  if (it != element.end())
    instance_name = it.value();

  it = element.find("configuration_file");
  if (it != element.end())
    instance_config = it.value();

  it = element.find("run");
  if (it != element.end())
    run = it.value();

  it = element.find("reload");
  if (it != element.end())
    reload = it.value();

  if (!instance_name.is_string())
    throw msg_fmt("name field not provided for cbd instance");
  if (!instance_config.is_string())
    throw msg_fmt("instance_config field not provided for cbd instance");
  if (!run.is_boolean())
    throw msg_fmt("run field not provided for cbd instance");

  if (!reload.is_boolean())
    throw msg_fmt("reload field not provided for cbd instance");

  if (instance_name.get<std::string>().empty())
    throw msg_fmt("missing instance name");

  if (executable.empty())
    executable = std::string(PREFIX_BIN "/cbd");

  if (!_instances_configuration
           .insert({instance_name.get<std::string>(),
                    instance_configuration(
                        instance_name.get<std::string>(), executable,
                        instance_config.get<std::string>(), run.get<bool>(),
                        reload.get<bool>(), 0)})
           .second) {
    std::string str(fmt::format("instance '{}' already exists",
                                instance_name.get<std::string>()));
    throw msg_fmt(str);
  }
}
