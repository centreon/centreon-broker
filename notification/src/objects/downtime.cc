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

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
downtime::downtime() :
  _entry_time(0),
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

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
downtime::downtime(downtime const& obj) {
  downtime::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
downtime& downtime::operator=(downtime const& obj) {
  if (this != &obj) {
    _entry_time = obj._entry_time;
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

/**
 *  Get the entry time of this downtime.
 *
 *  @return[in]  The entry time of this downtime.
 */
time_t downtime::get_entry_time() const throw() {
  return (_entry_time);
}

/**
 *  Set the entry time of this downtime.
 *
 *  @param[in] val  The new entry time of this downtime.
 */
void downtime::set_entry_time(time_t val) throw() {
  _entry_time = val;
}

/**
 *  Get the author of this downtime.
 *
 *  @return  The author of this downtime.
 */
std::string const& downtime::get_author() const throw() {
  return (_author);
}

/**
 *  Set the author of this downtime.
 *
 *  @param[in] val  The new author.
 */
void downtime::set_author(std::string const& val) {
  _author = val;
}

/**
 *  Was this downtime cancelled?
 *
 *  @return  True if this downtime was cancelled.
 */
bool downtime::get_cancelled() const throw() {
  return (_cancelled);
}

/**
 *  Set the downtime was cancelled flag.
 *
 *  @param[in] val  Was this downtime cancelled?
 */
void downtime::set_cancelled(bool val) throw() {
  _cancelled = val;
}

/**
 *  Get the deletion time of this downtime.
 *
 *  @return  The deletion time of this downtime.
 */
time_t downtime::get_deletion_time() const throw() {
  return (_deletion_time);
}

/**
 *  Set the deletion time of this downtime.
 *
 *  @param[in] val  The new value of the deletion time of this downtime.
 */
void downtime::set_deletion_time(time_t val) throw() {
  _deletion_time = val;
}

/**
 *  Get the duration of this downtime.
 *
 *  @return  The duration of this downtime.
 */
unsigned int downtime::get_duration() const throw() {
  return (_duration);
}

/**
 *  Set the duration of this downtime.
 *
 *  @param[in] val  The new duration of this downtime.
 */
void downtime::set_duration(unsigned int val) throw() {
  _duration = val;
}

/**
 *  Get the end time of this downtime.
 *
 *  @return  The end time of this downtime.
 */
time_t downtime::get_end_time() const throw() {
  return (_end_time);
}

/**
 *  Set the end time of this downtime.
 *
 *  @param[in] val  The new value for the end time of this downtime.
 */
void downtime::set_end_time(time_t val) throw() {
  _end_time = val;
}

/**
 *  Is this downtime fixed?
 *
 *  @return  True if this downtime is fixed.
 */
bool downtime::get_fixed() const throw() {
  return (_fixed);
}

/**
 *  Set the downtime is fixed flag.
 *
 *  @param[in] val  The new value for the downtime is fixed flag.
 */
void downtime::set_fixed(bool val) throw() {
  _fixed = val;
}

/**
 *  Get the start time of this downtime.
 *
 *  @return  The start time of this downtime.
 */
time_t downtime::get_start_time() const throw() {
  return (_start_time);
}

/**
 *  Set the start time of this downtime.
 *
 *  @param[in] val  The new value for the start time of this downtime.
 */
void downtime::set_start_time(time_t val) throw() {
  _start_time = val;
}

/**
 *  Get the actual start time of this downtime.
 *
 *  @return  The actual start time of this downtime.
 */
time_t downtime::get_actual_start_time() const throw() {
  return (_actual_start_time);
}

/**
 *  Set the actual start time of this downtime.
 *
 *  @param[in] val  The new value for the actual start time of this downtime.
 */
void downtime::set_actual_start_time(time_t val) throw() {
  _actual_start_time = val;
}

/**
 *  Get the actual end time of this downtime.
 *
 *  @return  The actual end time of this downtime.
 */
time_t downtime::get_actual_end_time() const throw() {
  return (_actual_end_time);
}

/**
 *  Set the actual end time of this downtime.
 *
 *  @param[in] val The new value for the actual end time of this downtime.
 */
void downtime::set_actual_end_time(time_t val) throw() {
  _actual_end_time = val;
}

/**
 *  Has this downtime started?
 *
 *  @return  True if this downtime has started.
 */
bool downtime::get_started() const throw() {
  return (_started);
}

/**
 *  Set the downtime has started flag.
 *
 *  @param[in] val  The new value for the downtime has started flag.
 */
void downtime::set_started(bool val) throw() {
  _started = val;
}

/**
 *  Return the id that triggered this downtime.
 *
 *  @return  The id that triggered this downtime.
 */
unsigned int downtime::get_triggered_by() const throw() {
  return (_triggered_by);
}

/**
 *  Set the id that triggered this downtime.
 *
 *  @param[in] val  The new value of the id that triggered this downtime.
 */
void downtime::set_triggered_by(unsigned int val) throw() {
  _triggered_by = val;
}

/**
 *  Get the type of this downtime.
 *
 *  @return  The type of this downtime.
 */
int downtime::get_type() const throw() {
  return (_type);
}

/**
 *  Set the type of this downtime.
 *
 *  @param[in] val  The new value for the type of this downtime.
 */
void downtime::set_type(int val) throw() {
  _type = val;
}
