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

#include "com/centreon/broker/notification/utilities/data_loggers.hh"

using namespace com::centreon::broker;

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::node_id const& node) throw()
{
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp << "node_id::get_host_id() : "
       << node.get_host_id()
       << "node_id::get_service_id() : "
       << node.get_service_id();
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::command const& com) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp << "command::get_name() : "
       << com.get_name();
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::downtime const& down) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::escalation const& esc) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::dependency const&) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::contact const&) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::timeperiod const&) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::node const&) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}

logging::temp_logger& logging::operator<<(logging::temp_logger const& left,
                                          notification::acknowledgement const&) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  return (tmp);
}
