/*
** Copyright 2013,2015 Merethis
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
 *  Get class instance.
 *
 *  @return Class instance.
 */
events& events::instance() {
  return (*_instance);
}

/**
 *  Load singleton.
 */
void events::load() {
  if (!_instance)
    _instance = new events;
  return ;
}

/**
 *  Unload singleton.
 */
void events::unload() {
  // Delete operator is NULL-aware.
  delete _instance;
  _instance = NULL;
  return ;
}

/**
 *  Register a category.
 *
 *  @param[in] name  Categoy name.
 *  @param[in] hint  Whished category ID.
 *
 *  @return Assigned category ID. This could be different than hint.
 */
unsigned short events::register_category(
                         std::string const& name,
                         unsigned short hint) {
  if (!hint)
    ++hint;
  while (_elements.find(hint) != _elements.end()) {
    if (!++hint)
      ++hint;
  }
  _elements[hint].name = name;
  return (hint);
}

/**
 *  Unregister a category.
 *
 *  @param[in] category_id  Category ID.
 */
void events::unregister_category(unsigned short category_id) {
  categories_container::iterator it(_elements.find(category_id));
  if (it != _elements.end())
    _elements.erase(it);
  return ;
}

/**
 *  Register an event.
 *
 *  @param[in] category_id  Category ID. Category must have been
 *                          registered through register_category().
 *  @param[in] event_id     Event ID whithin the category.
 *  @param[in] info         Information about the event.
 *
 *  @return Event type ID.
 */
unsigned int events::register_event(
                       unsigned short category_id,
                       unsigned short event_id,
                       event_info const& info) {
  categories_container::iterator it(_elements.find(category_id));
  if (it == _elements.end())
    throw (exceptions::msg() << "core: could not register event '"
           << info.get_name() << "': category " << category_id
           << " was not registered");
  int type(make_type(category_id, event_id));
  it->second.events[type] = info;
  return (type);
}

/**
 *  Unregister an event.
 *
 *  @param[in] type_id  Type ID.
 */
void events::unregister_event(unsigned int type_id) {
  unsigned short category_id(category_of_type(type_id));
  categories_container::iterator itc(_elements.find(category_id));
  if (itc != _elements.end()) {
    events_container::iterator ite(itc->second.events.find(type_id));
    if (ite != itc->second.end())
      itc->second.events.erase(ite);
  }
  return ;
}

/**
 *  Get first iterator.
 *
 *  @return First iterator.
 */
events::categories_container::const_iterator events::begin() const {
  return (_elements.begin());
}

/**
 *  Get last iterator.
 *
 *  @return Last iterator.
 */
events::categories_container::const_iterator events::end() const {
  return (_elements.end());
}

/**
 *  Get the content of a category.
 *
 *  @param[in] name  Category name.
 *
 *  @return Category elements.
 */
events::events_container const& events::get_events_by_category_name(
                                          std::string const& name) const {
  for (categories_container::const_iterator
         it(_elements.begin()),
         end(_elements.end());
       it != end;
       ++it) {
    if (it->second.name == name)
      return (it->second.events);
  }
  throw (exceptions::msg() << "core: cannot find event category '"
         << name << "'");
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
