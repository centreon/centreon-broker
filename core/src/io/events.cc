/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

// Class instance.
static events* _instance(NULL);

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Get first iterator.
 *
 *  @return First iterator.
 */
std::map<std::string, std::set<unsigned int> >::const_iterator events::begin() const {
  return (_elements.begin());
}

/**
 *  Get category of a type.
 *
 *  @param[in] type Type.
 *
 *  @return Category of type.
 */
unsigned short events::category_of_type(unsigned int type) throw () {
  return (static_cast<unsigned short>(type >> 16));
}

/**
 *  Get element of type.
 *
 *  @param[in] type Type.
 *
 *  @return Element of type.
 */
unsigned short events::element_of_type(unsigned int type) throw () {
  return (static_cast<unsigned short>(type));
}

/**
 *  Get last iterator.
 *
 *  @return Last iterator.
 */
std::map<std::string, std::set<unsigned int> >::const_iterator events::end() const {
  return (_elements.end());
}

/**
 *  Get the content of a category.
 *
 *  @param[in] name Category name.
 *
 *  @return Category elements.
 */
std::set<unsigned int> const& events::get(
                                        std::string const& name) const {
  std::map<std::string, std::set<unsigned int> >::const_iterator
    it(_elements.find(name));
  if (it == _elements.end())
    throw (exceptions::msg() << "core: cannot find event category '"
           << name << "'");
  return (it->second);
}

/**
 *  Get the class instance.
 *
 *  @return Class instance.
 */
events& events::instance() {
  return (*_instance);
}

/**
 *  Load the singleton.
 */
void events::load() {
  if (!_instance)
    _instance = new events;
  return ;
}

/**
 *  Register a new category.
 *
 *  @param[in] name     Category name.
 *  @param[in] category Category value.
 *  @param[in] elems    Category elements.
 */
void events::reg(
               std::string const& name,
               std::set<unsigned int> const& elems) {
  _elements[name] = elems;
  return ;
}

/**
 *  Unload the singleton.
 */
void events::unload() {
  delete _instance;
  _instance = NULL;
  return ;
}

/**
 *  Unregister a category.
 *
 *  @param[in] category Category name.
 */
void events::unreg(std::string const& name) {
  _elements.erase(name);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
events::events() {}

/**
 *  Destructor.
 */
events::~events() {}
