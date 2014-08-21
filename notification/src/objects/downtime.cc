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

#include "com/centreon/broker/notification/objects/downtime.hh"

using namespace com::centreon::broker::notification;

downtime::downtime() :
  _entry_time(0),
  _host_id(0),
  _service_id(0),
  _cancelled(true),
  _deletion_time(0),
  _duration(0),
  _end_time(0),
  _fixed(false),
  _start_time(0),
  _actual_start_time(0),
  _actual_end_time(0),
  _started(false),
  _triggered_by(0),
  _type(0) {}

downtime::downtime(downtime const& obj) :
  _entry_time(obj._entry_time),
  _host_id(obj._host_id),
  _service_id(obj._service_id),
  _cancelled(obj._cancelled),
  _deletion_time(obj._deletion_time),
  _duration(obj._duration),
  _end_time(obj._end_time),
  _fixed(obj._fixed),
  _start_time(obj._start_time),
  _actual_start_time(obj._actual_start_time),
  _actual_end_time(obj._actual_end_time),
  _started(obj._started),
  _triggered_by(obj._triggered_by),
  _type(obj._type) {}

downtime& downtime::operator=(downtime const& obj) {
  if (this != &obj) {
    _entry_time = obj._entry_time;
    _host_id = obj._host_id;
    _service_id = obj._service_id;
    _cancelled = obj._cancelled;
    _deletion_time = obj._deletion_time;
    _duration = obj._duration;
    _end_time = obj._end_time;
    _fixed = obj._fixed;
    _start_time = obj._start_time;
    _actual_start_time = obj._actual_start_time;
    _actual_end_time = obj._actual_end_time;
    _started = obj._started;
    _triggered_by = obj._triggered_by;
    _type = obj._type;
  }
  return (*this);
}

time_t downtime::get_entry_time() const throw() {
  return (_entry_time);
}

void downtime::set_entry_time(time_t val) throw() {
  _entry_time = val;
}

unsigned int downtime::get_host_id() const throw() {
  return (_host_id);
}

void downtime::set_host_id(unsigned int val) throw() {
  _host_id = val;
}

unsigned int downtime::get_service_id() const throw() {
  return (_service_id);
}

void downtime::set_service_id(unsigned int val) throw() {
  _service_id = val;
}

std::string const& downtime::get_author() const throw() {
  return (_author);
}

void downtime::set_author(std::string const& val) {
  _author = val;
}

bool downtime::get_cancelled() const throw() {
  return (_cancelled);
}

void downtime::set_cancelled(bool val) throw() {
  _cancelled = val;
}

time_t downtime::get_deletion_time() const throw() {
  return (_deletion_time);
}

void downtime::set_deletion_time(time_t val) throw() {
  _deletion_time = val;
}

unsigned int downtime::get_duration() const throw() {
  return (_duration);
}

void downtime::set_duration(unsigned int val) throw() {
  _duration = val;
}

time_t downtime::get_end_time() const throw() {
  return (_end_time);
}

void downtime::set_end_time(time_t val) throw() {
  _end_time = val;
}

bool downtime::get_fixed() const throw() {
  return (_fixed);
}

void downtime::set_fixed(bool val) throw() {
  _fixed = val;
}

time_t downtime::get_start_time() const throw() {
  return (_start_time);
}

void downtime::set_start_time(time_t val) throw() {
  _start_time = val;
}

time_t downtime::get_actual_start_time() const throw() {
  return (_actual_start_time);
}

void downtime::set_actual_start_time(time_t val) throw() {
  _actual_start_time = val;
}

time_t downtime::get_actual_end_time() const throw() {
  return (_actual_end_time);
}

void downtime::set_actual_end_time(time_t val) throw() {
  _actual_end_time = val;
}

bool downtime::get_started() const throw() {
  return (_started);
}

void downtime::set_started(bool val) throw() {
  _started = val;
}

unsigned int downtime::get_triggered_by() const throw() {
  return (_triggered_by);
}

void downtime::set_triggered_by(unsigned int val) throw() {
  _triggered_by = val;
}

int downtime::get_type() const throw() {
  return (_type);
}

void downtime::set_type(int val) throw() {
  _type = val;
}
