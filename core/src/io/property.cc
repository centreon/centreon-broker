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
 *  @param[in] name       The name of the property.
 *  @param[in] value      The value of the property..
 *  @param[in] graphable  If the plugin is graphable.
 */
property::property(std::string const& name,
                   std::string const& value,
                   bool graphable)
    : _name(name), _value(value), _graphable(graphable) {}

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
property::~property() throw() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
property& property::operator=(property const& right) {
  if (this != &right) {
    _name = right._name;
    _value = right._value;
    _graphable = right._graphable;
  }
  return (*this);
}

/**
 *  Get the name of this property.
 *
 *  @return  The name of this property.
 */
std::string const& property::get_name() const throw() {
  return (_name);
}

/**
 *  Get the value of this property.
 *
 *  @return  The value of this property.
 */
std::string const& property::get_value() const throw() {
  return (_value);
}

/**
 *  Get if the plugin is graphable.
 *
 *  @return True if the plugin is graphable, otherwise false.
 */
bool property::is_graphable() const throw() {
  return (_graphable);
}

/**
 *  Set the name of this property.
 *
 *  @param[in] name  The name of this property.
 */
void property::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Set the value of this property.
 *
 *  @param[in] value  The value of this property.
 */
void property::set_value(std::string const& value) {
  _value = value;
}

/**
 *  Set if graphable.
 *
 *  @param[in] graphable  True if graphable.
 */
void property::set_graphable(bool graphable) {
  _graphable = graphable;
}
