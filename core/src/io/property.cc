/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/io/property.hh"

using namespace com::centreon::broker::io;

/**
 *  Constructor.
 *
 *  @param[in] output     The plugin output.
 *  @param[in] perfdata   The plugin perfdata.
 *  @param[in] graphable  If the plugin is graphable.
 */
property::property(
            std::string const& output,
            std::string const& perfdata,
            bool graphable)
  : _graphable(graphable),
    _output(output),
    _perfdata(perfdata) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
property::property(property const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
property::~property() throw () {

}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
property& property::operator=(property const& right) {
  if (this != &right) {
    _graphable = right._graphable;
    _output = right._output;
    _perfdata = right._perfdata;
  }
  return (*this);
}

/**
 *  Get the plugin output.
 *
 *  @return The plugin output.
 */
std::string const& property::get_output() const throw () {
  return (_output);
}

/**
 *  Get the plugin perfdata.
 *
 *  @return The perfdata.
 */
std::string const& property::get_perfdata() const throw () {
  return (_perfdata);
}

/**
 *  Get if the plugin is graphable.
 *
 *  @return True if the plugin is graphable, otherwise false.
 */
bool property::is_graphable() const throw () {
  return (_graphable);
}

/**
 *  Set if the plugin is graphable.
 *
 *  @param[in] graphable True if the plugin is graphable.
 */
void property::set_graphable(bool graphable) {
  _graphable = graphable;
}

/**
 *  Set the plugin output.
 *
 *  @param[in] output The plugin output.
 */
void property::set_output(std::string const& output) {
  _output = output;
}

/**
 *  Set the plugin perfdata.
 *
 *  @param[in] perfdata The plugin perfdata.
 */
void property::set_perfdata(std::string const& perfdata) {
  _perfdata = perfdata;
}
