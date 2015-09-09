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
