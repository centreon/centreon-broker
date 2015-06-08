/*
** Copyright 2009-2013,2015 Merethis
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

#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/downtime_serializable.hh"

using namespace com::centreon::broker::neb;

/**
 *  Stream operator for QString.
 *
 *  @param[in] stream    The stream.
 *  @param[in] fake_str  The QString.
 *
 *  @return            Reference to the stream.
 */
std::istream& com::centreon::broker::neb::operator>>(
                std::istream& stream, QString& fake_str) {
  std::string str;
  stream >> str;
  fake_str = QString::fromStdString(str);
  return (stream);
}

/**
 *  Stream operator for QString.
 *
 *  @param[in] stream    The stream.
 *  @param[in] fake_str  The QString.
 *
 *  @return            Reference to the stream.
 */
std::ostream& com::centreon::broker::neb::operator<<(
                std::ostream& stream, QString const& fake_str) {
  std::string str = fake_str.toStdString();
  stream << str;
  return (stream);
}

/**
 *  Default constructor.
 */
downtime_serializable::downtime_serializable()
  : ceof_serializable<downtime_serializable>(),
    _downtime(new downtime){
  _downtime->come_from = 1;
  init_bindings();
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
downtime_serializable::downtime_serializable(downtime_serializable const& other)
  : ceof_serializable<downtime_serializable>(other),
    _downtime(new downtime(*other._downtime)) {
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           A reference to this object.
 */
downtime_serializable& downtime_serializable::operator=(
                         downtime_serializable const& other) {
  if (this != &other)
    _downtime = misc::make_shared(new downtime(*other._downtime));
  return (*this);
}

/**
 *  Destructor.
 */
downtime_serializable::~downtime_serializable() {

}

template <typename U, U (downtime::* member)>
std::string downtime_serializable::get_downtime_member() const {
  std::stringstream ss;
  ss << ((*_downtime).*member);
  return (ss.str());
}

template <typename U, U (downtime::* member)>
void downtime_serializable::set_downtime_member(std::string const& val) {
  std::stringstream ss;
  ss << val;
  ss >> ((*_downtime).*member);
}

/**
 *  Get the underlying downtime.
 *
 *  @return  Return the downtime.
 */
com::centreon::broker::misc::shared_ptr<downtime>
  downtime_serializable::get_downtime() const {
  return (_downtime);
}

/**
 *  Init the bindings.
 */
void downtime_serializable::init_bindings() {
  add_member(
    "actual_end_time",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::actual_end_time>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::actual_end_time>);
  add_member(
    "actual_start_time",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::actual_start_time>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::actual_start_time>);
  add_member(
    "author",
    &downtime_serializable::get_downtime_member<QString, &downtime::author>,
    &downtime_serializable::set_downtime_member<QString, &downtime::author>);
  add_member(
    "comment",
    &downtime_serializable::get_downtime_member<QString, &downtime::comment>,
    &downtime_serializable::set_downtime_member<QString, &downtime::comment>);
  add_member(
    "deletion_time",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::deletion_time>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::deletion_time>);
  add_member(
    "downtime_type",
    &downtime_serializable::get_downtime_member<short, &downtime::downtime_type>,
    &downtime_serializable::set_downtime_member<short, &downtime::downtime_type>);
  add_member(
    "duration",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::duration>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::duration>);
  add_member(
    "end_time",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::end_time>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::end_time>);
  add_member(
    "entry_time",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::entry_time>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::entry_time>);
  add_member(
    "fixed",
    &downtime_serializable::get_downtime_member<bool, &downtime::fixed>,
    &downtime_serializable::set_downtime_member<bool, &downtime::fixed>);
  add_member(
    "host_id",
    &downtime_serializable::get_downtime_member<unsigned int, &downtime::host_id>,
    &downtime_serializable::set_downtime_member<unsigned int, &downtime::host_id>);
  add_member(
    "internal_id",
    &downtime_serializable::get_downtime_member<unsigned int, &downtime::internal_id>,
    &downtime_serializable::set_downtime_member<unsigned int, &downtime::internal_id>);
  add_member(
    "service_id",
    &downtime_serializable::get_downtime_member<unsigned int, &downtime::service_id>,
    &downtime_serializable::set_downtime_member<unsigned int, &downtime::service_id>);
  add_member(
    "start_time",
    &downtime_serializable::get_downtime_member<timestamp, &downtime::start_time>,
    &downtime_serializable::set_downtime_member<timestamp, &downtime::start_time>);
  add_member(
    "triggered_by",
    &downtime_serializable::get_downtime_member<unsigned int, &downtime::triggered_by>,
    &downtime_serializable::set_downtime_member<unsigned int, &downtime::triggered_by>);
  add_member(
    "was_cancelled",
    &downtime_serializable::get_downtime_member<bool, &downtime::was_cancelled>,
    &downtime_serializable::set_downtime_member<bool, &downtime::was_cancelled>);
  add_member(
    "was_started",
    &downtime_serializable::get_downtime_member<bool, &downtime::was_started>,
    &downtime_serializable::set_downtime_member<bool, &downtime::was_started>);
  add_member(
    "is_reccuring",
    &downtime_serializable::get_downtime_member<bool, &downtime::is_recurring>,
    &downtime_serializable::set_downtime_member<bool, &downtime::is_recurring>);
  add_member(
    "recurring_timeperiod",
    &downtime_serializable::get_downtime_member<QString, &downtime::recurring_timeperiod>,
    &downtime_serializable::set_downtime_member<QString, &downtime::recurring_timeperiod>);
}
