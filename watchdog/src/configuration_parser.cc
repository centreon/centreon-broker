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

#include <QFile>
#include "com/centreon/broker/watchdog/configuration_parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

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
  QFile config_file(config_filename.c_str());
  if (!config_file.open(QFile::ReadOnly))
    // We don't know where is our log file, so we can't log.
    throw exceptions::msg()
      << "watchdog: cannot open '" << config_filename
      << "': " << config_file.errorString();

  // Parse the configuration file.
  QString error_msg;
  int error_line;
  int error_column;
  if (!_xml_document.setContent(
                       &config_file,
                       &error_msg,
                       &error_line,
                       &error_column))
    throw exceptions::msg()
      << "watchdog: couldn't parse file '" << config_filename << "': "
      << error_msg << " at line '" << error_line << "', column '"
      <<  error_column << "'";
}

/**
 *  Parse the xml document.
 */
void configuration_parser::_parse_xml_document() {
  QDomElement e = _xml_document.firstChildElement();
  while(!e.isNull()) {
      if (e.tagName() == "log")
        _log_path = e.text();
      else if (e.tagName() == "cbd")
        _parse_centreon_broker_element(e);
     QDomElement e = e.nextSiblingElement();
  }
}

/**
 *  Parse an xml element containing a centreon broker declaration.
 *
 *  @param[in] element  The element.
 */
void configuration_parser::_parse_centreon_broker_element(
                             QDomElement const& element) {
  // The default are sane.
  QString instance_name = element.firstChildElement("instance_name").text();
  QString instance_config = element.firstChildElement("instance_config").text();
  bool run = (element.firstChildElement("run").text() == "true");
  bool reload = (element.firstChildElement("reload").text() == "true");
  unsigned int seconds_per_tentative
    = (element.firstChildElement("seconds_per_tentative").text().toUInt());

  if (instance_name.isEmpty())
    throw (exceptions::msg() << "watchdog: missing instance_name");

  if (_instances_configuration.insert(
    std::make_pair(
           instance_name.toStdString(),
           broker_instance_configuration(
             instance_name.toStdString(),
             instance_config.toStdString(),
             run,
             reload,
             seconds_per_tentative))).second == false)
    throw (exceptions::msg()
             << "watchdog: instance '" << instance_name << "' already exists");
}
