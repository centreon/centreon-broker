/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/objects/timeperiod.hh"

using namespace com::centreon::broker::notification;

timeperiod::timeperiod() {}

timeperiod::timeperiod(timeperiod const& obj) :
  _alias(obj._alias),
  _exceptions(obj._exceptions),
  _exclude(obj._exclude),
  _timeperiod_name(obj._timeperiod_name),
  _timeranges(obj._timeranges) {}

timeperiod timeperiod::operator=(timeperiod const& obj) {
  if (this != &obj) {
    _alias = obj._alias;
    _exceptions = obj._exceptions;
    _exclude = obj._exclude;
    _timeperiod_name = obj._timeperiod_name;
    _timeranges = obj._timeranges;
  }
  return (*this);
}

std::string const& timeperiod::get_alias() const throw() {
  return (_alias);
}

void timeperiod::set_alias(std::string const& value) {
  _alias = value;
}

std::vector<std::list<daterange> > const&
  timeperiod::get_exceptions() const throw() {
  return (_exceptions);
}

void timeperiod::add_exceptions(std::list<daterange> const& val) {
  _exceptions.push_back(val);
}

group const& timeperiod::get_exclude() const throw() {
  return (_exclude);
}

void timeperiod::set_exclude(group const& val) {
  _exclude = val;
}

std::string const& timeperiod::get_timeperiod_name() const throw() {
  return (_timeperiod_name);
}

void timeperiod::set_timeperiod_name(std::string const& value) {
  _timeperiod_name = value;
}

std::vector<std::list<timerange> > const&
  timeperiod::get_timeranges() const throw() {
  return (_timeranges);
}

void timeperiod::add_timerange(std::list<timerange> const& val) {
  _timeranges.push_back(val);
}
