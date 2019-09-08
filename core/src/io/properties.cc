/*
** Copyright 2013-2014 Centreon
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

#include "com/centreon/broker/io/properties.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::io;

/**
 *  Constructor.
 *
 *  @param[in] name The properties name.
 */
properties::properties(std::string const& name) : _name(name) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
properties::properties(properties const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
properties::~properties() throw() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
properties& properties::operator=(properties const& right) {
  if (this != &right) {
    _children = right._children;
    _name = right._name;
    _properties = right._properties;
  }
  return (*this);
}

/**
 *  Get property by name.
 *
 *  @param[in] name The property name to get.
 *
 *  @return The property or throw exception.
 */
property const& properties::operator[](std::string const& name) const {
  return (get(name));
}

/**
 *  Get property by name.
 *
 *  @param[in] name The property name to get.
 *
 *  @return The property or insert new property.
 */
property& properties::operator[](std::string const& name) {
  return (_properties[name]);
}

/**
 *  Get the first iterator.
 *
 *  @return First iterator.
 */
properties::iterator properties::begin() {
  return (_properties.begin());
}

/**
 *  Get the first iterator.
 *
 *  @return First iterator.
 */
properties::const_iterator properties::begin() const {
  return (_properties.begin());
}

/**
 *  Get the children list.
 *
 *  @return Children.
 */
properties::children_list const& properties::children() const {
  return (_children);
}

/**
 *  Get the children list.
 *
 *  @return Children.
 */
properties::children_list& properties::children() {
  return (_children);
}

/**
 *  Add a child tree to this property tree.
 *
 *  @param[in] child  The child to add to this property tree.
 *  @param[in] name   The name of the child tree.
 */
void properties::add_child(properties const& child, std::string const& name) {
  _children.push_back(std::make_pair(name, child));
}

/**
 *  Get the last iterator.
 *
 *  @return Last iterator.
 */
properties::iterator properties::end() {
  return (_properties.end());
}

/**
 *  Get the last iterator.
 *
 *  @return Last iterator.
 */
properties::const_iterator properties::end() const {
  return (_properties.end());
}

/**
 *  Get property by name.
 *
 *  @param[in] name The property name to get.
 *
 *  @return The property or throw exception.
 */
property const& properties::get(std::string const& name) const {
  std::map<std::string, property>::const_iterator it(_properties.find(name));
  if (it == _properties.end())
    throw(exceptions::msg() << "property '" << name << "' not found");
  return (it->second);
}

/**
 *  Merge new properties in this object.
 *
 *  @param[in] other Properties that will be merged in this object.
 */
void properties::merge(properties const& other) {
  for (const_iterator it(other.begin()), end(other.end()); it != end; ++it)
    _properties[it->first] = it->second;
  return;
}

/**
 *  Get the properties name.
 *
 *  @return The name.
 */
std::string const& properties::name() const throw() {
  return (_name);
}

/**
 *  Set the properties name.
 *
 *  @param[in] name The properties name.
 */
void properties::name(std::string const& name) {
  _name = name;
}

/**
 *  Add a property.
 *
 *  @param[in] name  The name of the property.
 *  @param[in] prop  The property.
 */
void properties::add_property(std::string const& name, property const& prop) {
  (*this)[name] = prop;
}
