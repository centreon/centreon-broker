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

#include "com/centreon/broker/neb/downtime_serializable.hh"
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
downtime_serializable::downtime_serializable() : _downtime(new downtime) {
  _downtime->come_from = 1;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
downtime_serializable::downtime_serializable(downtime_serializable const& other)
    : _downtime(new downtime(*other._downtime)) {}

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
    _downtime.reset(new downtime(*other._downtime));
  return *this;
}

/**
 *  Destructor.
 */
downtime_serializable::~downtime_serializable() {}

template <typename U, U(downtime::*member)>
std::string downtime_serializable::get_downtime_member() const {
  std::stringstream ss;
  ss << ((*_downtime).*member);
  return ss.str();
}

template <typename U, U(downtime::*member)>
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
std::shared_ptr<downtime> downtime_serializable::get_downtime() const {
  return _downtime;
}

/**
 *   Visit.
 *
 *   @param[in] visitor  The visitor.
 */
void downtime_serializable::visit(ceof::ceof_visitor& visitor) {
  visitor.visit(
      *this, "actual_end_time",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::actual_end_time>,
      &downtime_serializable::set_downtime_member<timestamp,
                                                  &downtime::actual_end_time>);
  visitor.visit(
      *this, "actual_start_time",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::actual_start_time>,
      &downtime_serializable::set_downtime_member<
          timestamp, &downtime::actual_start_time>);
  visitor.visit(*this, "author",
                &downtime_serializable::get_downtime_member<std::string,
                                                            &downtime::author>,
                &downtime_serializable::set_downtime_member<std::string,
                                                            &downtime::author>);
  visitor.visit(
      *this, "comment",
      &downtime_serializable::get_downtime_member<std::string,
                                                  &downtime::comment>,
      &downtime_serializable::set_downtime_member<std::string,
                                                  &downtime::comment>);
  visitor.visit(
      *this, "deletion_time",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::deletion_time>,
      &downtime_serializable::set_downtime_member<timestamp,
                                                  &downtime::deletion_time>);
  visitor.visit(
      *this, "downtime_type",
      &downtime_serializable::get_downtime_member<short,
                                                  &downtime::downtime_type>,
      &downtime_serializable::set_downtime_member<short,
                                                  &downtime::downtime_type>);
  visitor.visit(
      *this, "duration",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::duration>,
      &downtime_serializable::set_downtime_member<timestamp,
                                                  &downtime::duration>);
  visitor.visit(
      *this, "end_time",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::end_time>,
      &downtime_serializable::set_downtime_member<timestamp,
                                                  &downtime::end_time>);
  visitor.visit(
      *this, "entry_time",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::entry_time>,
      &downtime_serializable::set_downtime_member<timestamp,
                                                  &downtime::entry_time>);
  visitor.visit(
      *this, "fixed",
      &downtime_serializable::get_downtime_member<bool, &downtime::fixed>,
      &downtime_serializable::set_downtime_member<bool, &downtime::fixed>);
  visitor.visit(
      *this, "host_id",
      &downtime_serializable::get_downtime_member<uint32_t,
                                                  &downtime::host_id>,
      &downtime_serializable::set_downtime_member<uint32_t,
                                                  &downtime::host_id>);
  visitor.visit(
      *this, "internal_id",
      &downtime_serializable::get_downtime_member<uint32_t,
                                                  &downtime::internal_id>,
      &downtime_serializable::set_downtime_member<uint32_t,
                                                  &downtime::internal_id>);
  visitor.visit(
      *this, "service_id",
      &downtime_serializable::get_downtime_member<uint32_t,
                                                  &downtime::service_id>,
      &downtime_serializable::set_downtime_member<uint32_t,
                                                  &downtime::service_id>);
  visitor.visit(
      *this, "start_time",
      &downtime_serializable::get_downtime_member<timestamp,
                                                  &downtime::start_time>,
      &downtime_serializable::set_downtime_member<timestamp,
                                                  &downtime::start_time>);
  visitor.visit(
      *this, "triggered_by",
      &downtime_serializable::get_downtime_member<uint32_t,
                                                  &downtime::triggered_by>,
      &downtime_serializable::set_downtime_member<uint32_t,
                                                  &downtime::triggered_by>);
  visitor.visit(
      *this, "was_cancelled",
      &downtime_serializable::get_downtime_member<bool,
                                                  &downtime::was_cancelled>,
      &downtime_serializable::set_downtime_member<bool,
                                                  &downtime::was_cancelled>);
  visitor.visit(
      *this, "was_started",
      &downtime_serializable::get_downtime_member<bool, &downtime::was_started>,
      &downtime_serializable::set_downtime_member<bool,
                                                  &downtime::was_started>);
  visitor.visit(
      *this, "is_recurring",
      &downtime_serializable::get_downtime_member<bool,
                                                  &downtime::is_recurring>,
      &downtime_serializable::set_downtime_member<bool,
                                                  &downtime::is_recurring>);
  visitor.visit(*this, "recurring_period",
                &downtime_serializable::get_downtime_member<
                    std::string, &downtime::recurring_timeperiod>,
                &downtime_serializable::set_downtime_member<
                    std::string, &downtime::recurring_timeperiod>);
}
