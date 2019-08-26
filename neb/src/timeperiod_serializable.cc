/*
** Copyright 2009-2013 Centreon
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

#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/timeperiod_serializable.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
timeperiod_serializable::timeperiod_serializable(
  std::unordered_map<std::string, time::timeperiod::ptr> const& tps)
  : _tps{&tps},
    _tp{new time::timeperiod()} {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
timeperiod_serializable::timeperiod_serializable(
                           timeperiod_serializable const& other)
  : _tps(other._tps),
    _tp(new time::timeperiod(*other._tp)){
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           A reference to this object.
 */
timeperiod_serializable& timeperiod_serializable::operator=(
                           timeperiod_serializable const& other) {
  if (this != &other) {
    _tps = other._tps;
    _tp.reset(new time::timeperiod(*other._tp));
  }
  return (*this);
}

/**
 *  Destructor.
 */
timeperiod_serializable::~timeperiod_serializable() {

}

/**
 *  Get the name of this timeperiod.
 *
 *  @return  The name of this timeperiod.
 */
std::string timeperiod_serializable::get_name() const {
  return (_tp->get_name());
}

/**
 *  Get the alias of this timeperiod.
 *
 *  @return  The alias of this timeperiod.
 */
std::string timeperiod_serializable::get_alias() const {
  return (_tp->get_alias());
}

/**
 *  Get the sunday timerange.
 *
 *  @return  The sunday timerange.
 */
std::string timeperiod_serializable::get_sunday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(0)));
}

/**
 *  Get the monday timerange.
 *
 *  @return  The monday timerange.
 */
std::string timeperiod_serializable::get_monday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(1)));
}

/**
 *  Get the tuesday timerange.
 *
 *  @return  The tuesday timerange.
 */
std::string timeperiod_serializable::get_tuesday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(2)));
}

/**
 *  Get the wednesday timerange.
 *
 *  @return  The wednesday timerange.
 */
std::string timeperiod_serializable::get_wednesday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(3)));
}

/**
 *  Get the thursday timerange.
 *
 *  @return  The thursday timerange.
 */
std::string timeperiod_serializable::get_thursday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(4)));
}

/**
 *  Get the friday timerange.
 *
 *  @return  The friday timerange.
 */
std::string timeperiod_serializable::get_friday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(5)));
}

/**
 *  Get the saturday timerange.
 *
 *  @return  The saturday timerange.
 */
std::string timeperiod_serializable::get_saturday() const {
  return (time::timerange::build_string_from_timeranges(
            _tp->get_timeranges_by_day(6)));
}

/**
 *  Get the exceptions of this timeperiod.
 *
 *  @return  The exceptions of this timeperiod.
 */
std::string timeperiod_serializable::get_exceptions() const {
  // TODO: when it will be actually needed.
  return ("");
}

/**
 *  Get the excluded timeperiods of this timeperiod.
 *
 *  @return  The excluded timeperiods of this timeperiod.
 */
std::string timeperiod_serializable::get_excluded() const {
  std::string ret;
  std::vector<time::timeperiod::ptr> const& excluded = _tp->get_excluded();
  for (std::vector<time::timeperiod::ptr> ::const_iterator
         it = excluded.begin(),
         end = excluded.end();
       it != end;
       ++it) {
    if (!ret.empty())
      ret.append(",");
    ret += (*it)->get_name();
  }
  return (ret);
}

/**
 *  Get the included timeperiods of this timeperiod.
 *
 *  @return  The included timeperiods of this timeperiod.
 */
std::string timeperiod_serializable::get_included() const {
  std::string ret;
  std::vector<time::timeperiod::ptr> const& included = _tp->get_included();
  for (std::vector<time::timeperiod::ptr> ::const_iterator
         it = included.begin(),
         end = included.end();
       it != end;
       ++it) {
    if (!ret.empty())
      ret.append(",");
    ret += (*it)->get_name();
  }
  return (ret);
}

/**
 *  Set the name of this timeperiod.
 *
 *  @param[in] name  The new name of this timeperiod.
 */
void timeperiod_serializable::set_name(std::string const& name) {
  _tp->set_name(name);
}

/**
 *  Set the alias of this timeperiod.
 *
 *  @param[in] name  The new alias of this timeperiod.
 */
void timeperiod_serializable::set_alias(std::string const& val) {
  _tp->set_alias(val);
}

/**
 *  Set the sunday timerange.
 *
 *  @param[in] val  The new sunday timerange.
 */
void timeperiod_serializable::set_sunday(std::string const& val) {
  if (!_tp->set_timerange(val, 0))
    throw (exceptions::msg()
           << "couldn't set sunday for " << _tp->get_name());
}

/**
 *  Set the monday timerange.
 *
 *  @param[in] val  The new monday timerange.
 */
void timeperiod_serializable::set_monday(std::string const& val) {
  if (!_tp->set_timerange(val, 1))
    throw (exceptions::msg()
           << "couldn't set monday for " << _tp->get_name());
}

/**
 *  Set the tuesday timerange.
 *
 *  @param[in] val  The new tuesday timerange.
 */
void timeperiod_serializable::set_tuesday(std::string const& val) {
  if (!_tp->set_timerange(val, 2))
    throw (exceptions::msg()
           << "couldn't set tuesday for " << _tp->get_name());
}

/**
 *  Set the wednesday timerange.
 *
 *  @param[in] val  The new wednesday timerange.
 */
void timeperiod_serializable::set_wednesday(std::string const& val) {
  if (!_tp->set_timerange(val, 3))
    throw (exceptions::msg()
           << "couldn't set wednesday for " << _tp->get_name());
}

/**
 *  Set the thursday timerange.
 *
 *  @param[in] val  The new thursday timerange.
 */
void timeperiod_serializable::set_thursday(std::string const& val) {
  if (!_tp->set_timerange(val, 4))
    throw (exceptions::msg()
           << "couldn't set thursday for " << _tp->get_name());
}

/**
 *  Set the friday timerange.
 *
 *  @param[in] val  The new friday timerange.
 */
void timeperiod_serializable::set_friday(std::string const& val) {
  if (!_tp->set_timerange(val, 5))
    throw (exceptions::msg()
           << "couldn't set friday for " << _tp->get_name());
}

/**
 *  Set the saturday timerange.
 *
 *  @param[in] val  The new saturday timerange.
 */
void timeperiod_serializable::set_saturday(std::string const& val) {
  if (!_tp->set_timerange(val, 6))
    throw (exceptions::msg()
           << "couldn't set saturday for " << _tp->get_name());
}

/**
 *  Set the exceptions of this timeperiod.
 *
 *  @param[in] val  The exceptions of this timeperiod.
 */
void timeperiod_serializable::set_exceptions(std::string const& val) {
  std::vector<std::list<time::daterange> > dateranges;
  if (time::daterange::build_dateranges_from_string(val, dateranges))
    throw (exceptions::msg()
           << "couldn't parse exceptions timeranges '" << val << "'");
  for (std::vector<std::list<time::daterange> >::const_iterator
         it = dateranges.begin(),
         end = dateranges.end();
       it != end;
       ++it)
    _tp->add_exceptions(*it);
}

/**
 *  Set the excluded timeperiods of this timeperiod.
 *
 *  @param[in] val  The excluded timeperiods.
 */
void timeperiod_serializable::set_excluded(std::string const& val) {
  std::vector<std::string> excluded;
  misc::string::split(val, excluded, ',');
  for (std::vector<std::string>::const_iterator
         it = excluded.begin(),
         end = excluded.end();
       it != end;
       ++it) {
    time::timeperiod::ptr tp = _tps->at(*it);
    if (!tp)
      throw (exceptions::msg()
             << "couldn't find the excluded timeperiod '" << *it << "'");
    _tp->add_excluded(tp);
  }
}

/**
 *  Set the included timeperiods of this timeperiod.
 *
 *  @param[in] val  The included timeperiods.
 */
void timeperiod_serializable::set_included(std::string const& val) {
  std::vector<std::string> included;
  misc::string::split(val, included, ',');
  for (std::vector<std::string>::const_iterator
         it = included.begin(),
         end = included.end();
       it != end;
       ++it) {
    time::timeperiod::ptr tp = _tps->at(*it);
    if (!tp)
      throw (exceptions::msg()
             << "couldn't find the included timeperiod '" << *it << "'");
    _tp->add_included(tp);
  }
}

/**
 *  Get the underlying timeperiod.
 *
 *  @return  The underlying timeperiod.
 */
com::centreon::broker::time::timeperiod::ptr
  timeperiod_serializable::get_timeperiod() const {
  return (_tp);
}

/**
 *   Visit the object.
 *
 *   @param[in] visitor  The visitor.
 */
void timeperiod_serializable::visit(ceof::ceof_visitor& visitor) {
  visitor.visit(
    *this,
    "timeperiod_name",
    &timeperiod_serializable::get_name,
    &timeperiod_serializable::set_name);
  visitor.visit(
    *this,
    "alias",
    &timeperiod_serializable::get_alias,
    &timeperiod_serializable::set_alias);
  visitor.visit(
    *this,
    "sunday",
    &timeperiod_serializable::get_sunday,
    &timeperiod_serializable::set_sunday);
  visitor.visit(
    *this,
    "monday",
    &timeperiod_serializable::get_monday,
    &timeperiod_serializable::set_monday);
  visitor.visit(
     *this,
    "tuesday",
     &timeperiod_serializable::get_tuesday,
     &timeperiod_serializable::set_tuesday);
  visitor.visit(
     *this,
     "wednesday",
     &timeperiod_serializable::get_wednesday,
     &timeperiod_serializable::set_wednesday);
  visitor.visit(
     *this,
     "thursday",
     &timeperiod_serializable::get_thursday,
     &timeperiod_serializable::set_thursday);
  visitor.visit(
     *this,
     "friday",
     &timeperiod_serializable::get_friday,
     &timeperiod_serializable::set_friday);
  visitor.visit(
     *this,
     "saturday",
     &timeperiod_serializable::get_saturday,
     &timeperiod_serializable::set_saturday);
  visitor.visit(
     *this,
     "exceptions",
     &timeperiod_serializable::get_exceptions,
     &timeperiod_serializable::set_exceptions);
  visitor.visit(
     *this,
     "exclude",
     &timeperiod_serializable::get_excluded,
     &timeperiod_serializable::set_excluded);
  visitor.visit(
     *this,
     "include",
     &timeperiod_serializable::get_included,
     &timeperiod_serializable::set_included);
}
