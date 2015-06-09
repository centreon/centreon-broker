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

#include <algorithm>
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
    if (ite != itc->second.events.end())
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
events::events_container events::get_events_by_category_name(
                                   std::string const& name) const {
  // Special category matching all registered events.
  if (name == "all") {
    events::events_container all;
    for (categories_container::const_iterator
           it1(_elements.begin()), end1(_elements.end());
         it1 != end1;
         ++it1)
      for (events_container::const_iterator
             it2(it1->second.events.begin()),
             end2(it1->second.events.end());
           it2 != end2;
           ++it2)
        all.insert(*it2);
    return (all);
  }
  // Category name.
  else {
    for (categories_container::const_iterator
           it(_elements.begin()), end(_elements.end());
         it != end;
         ++it) {
      if (it->second.name == name)
        return (it->second.events);
    }
  }
  throw (exceptions::msg() << "core: cannot find event category '"
         << name << "'");
}

/**
 *  Get an event information structure.
 *
 *  @param[in] type  Event type ID.
 *
 *  @return Event information structure if found, NULL otherwise.
 */
event_info const* events::get_event_info(unsigned int type) {
  umap<unsigned short, category_info>::const_iterator
    itc(_elements.find(category_of_type(type)));
  if (itc != _elements.end()) {
    umap<unsigned int, event_info>::const_iterator
      ite(itc->second.events.find(type));
    if (ite != itc->second.events.end())
      return (&ite->second);
  }
  return (NULL);
}

/**
 *  Get all the events matching this name.
 *
 *  If it's a category name, get the content of the category.
 *  If it's a category name followed by : and the name of an event,
 *  get this event.
 *
 *  @param[in] name  The name.
 *
 *  @return  A list of all the matching events.
 */
events::events_container events::get_matching_events(
                                   std::string const& name) const {
  // Ignore internal category.
  if (name.compare(0, ::strlen("internal"), "internal") == 0)
    return (events_container());

  size_t num = std::count(name.begin(), name.end(), ':');
  if (num == 0)
    return (get_events_by_category_name(name));
  else if (num == 1) {
    size_t place = name.find_first_of(':');
    std::string category_name = name.substr(0, place);
    events::events_container const &events = get_events_by_category_name(
                                               category_name);
    std::string event_name = name.substr(place + 1);
    for (events::events_container::const_iterator
           it(events.begin()),
           end(events.end());
         it != end;
         ++it) {
      if (it->second.get_name() == event_name) {
        events::events_container res;
        res[it->first] = it->second;
        return (res);
      }
    }
    throw (exceptions::msg() << "core: cannot find event '"
           << event_name << "' in '" << name << "'");
  }
  else
    throw (exceptions::msg() << "core: too many ':' in '"
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
events::events() {
  // Register internal category.
  register_category("internal", io::events::internal);
}

/**
 *  Destructor.
 */
events::~events() {
  unregister_category(io::events::internal);
}
