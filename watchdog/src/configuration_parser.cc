/*
** Copyright 2011-2013 Centreon
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

#include <json11.hpp>
#include <fstream>
#include <streambuf>
#include "com/centreon/broker/watchdog/configuration_parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "../../../../../../usr/include/qt4/QtCore/QString"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;
using namespace json11;

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
  _parse_xml_document();
  return (configuration(_log_path.toStdString(), _instances_configuration));
}

/**
 *  Parse a file into a xml document.
 *
 *  @param[in] config_filename  The config file name.
 */
void configuration_parser::_parse_file(std::string const& config_filename) {
  // Parse Json file
  std::ifstream f(config_filename);
  std::string const& json_to_parse{
    std::istreambuf_iterator<char>(f),
    std::istreambuf_iterator<char>()};
  std::string err;

  _json_document = Json::parse(json_to_parse, err);

  if (_json_document.is_null())
    throw exceptions::msg() << "config parser: cannot parse file '"
                            << config_filename << "': " << err;
  if (!_json_document.is_object() || !!_json_document["centreonBroker"].is_object())
    throw exceptions::msg() << "config parser: cannot parse file '"
                            << config_filename << "': it must contain a centreonBroker object";

  QFile config_file(config_filename.c_str());
  if (!config_file.open(QFile::ReadOnly))
    // We don't know where is our log file, so we can't log.
    throw exceptions::msg()
      << "cannot open '" << config_filename
      << "': " << config_file.errorString();
}

/**
 *  Parse the xml document.
 */
void configuration_parser::_parse_xml_document() {
  for (std::pair<std::string const, Json> const& object : _json_document["centreonBroker"].object_items()) {
    if (object.first == "log")
     _log_path = QString::fromStdString(object.second.string_value());
    else if (object.first == "cbd") {
      if (object.second.is_array())
        for (Json const &entry : object.second.array_items())
          _parse_centreon_broker_element(entry);
      else
        throw exceptions::msg()
          << "error in watchdog config syntax 'cbd' must be an array";
    }
  }
}

/**
 *  Parse an xml element containing a centreon broker declaration.
 *
 *  @param[in] element  The element.
 */
void configuration_parser::_parse_centreon_broker_element(
                             json11::Json const& element) {
  // The default are sane.
  Json const& instance_name{element["name"]};
  Json const& instance_config{element["configuration_file"]};
  Json const& run{element["run"]};
  Json const& reload{element["reload"]};

  if (!instance_name.is_string())
    throw exceptions::msg() << "name field not provided for cbd instance";
  if (!instance_config.is_string())
    throw exceptions::msg()
      << "instance_config field not provided for cbd instance";
  if (!run.is_bool())
    throw exceptions::msg() << "run field not provided for cbd instance";

  if (!reload.is_bool())
    throw exceptions::msg() << "reload field not provided for cbd instance";

  if (instance_name.string_value().empty())
    throw exceptions::msg() << "watchdog: missing instance_name";

  if (_instances_configuration.insert(
    std::make_pair(
           instance_name.string_value(),
           instance_configuration(
             instance_name.string_value(),
             instance_config.string_value(),
             run.bool_value(),
             reload.bool_value(),
             0))).second == false)
    throw exceptions::msg()
             << "instance '" << instance_name.string_value() << "' already exists";
}
