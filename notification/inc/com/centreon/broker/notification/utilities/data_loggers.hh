/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH
#  define CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH

#  include <string>
#  include <vector>
#  include <list>
#  include <deque>
#  include <map>
#  include <set>
#  include <QHash>
#  include <QSet>
#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/broker/logging/logging.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/command.hh"
#  include "com/centreon/broker/notification/objects/downtime.hh"
#  include "com/centreon/broker/notification/objects/dependency.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/objects/timeperiod.hh"
#  include "com/centreon/broker/notification/objects/node.hh"
#  include "com/centreon/broker/notification/objects/acknowledgement.hh"

CCB_BEGIN()

namespace logging {
  // Constant ref needed because the temp_logger returned by logging
  // is a rvalue, and can only be accessed by a const reference.
  // Am I pushing the boundary of the language too far?
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::node_id const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::command const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::downtime const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::dependency const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::contact const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::timeperiod const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::node const&) throw();
  temp_logger& operator<<(temp_logger const& left,
                          notification::objects::acknowledgement const&) throw();

  template <typename T>
  temp_logger& operator<<(temp_logger const& left,
                          std::vector<T> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename std::vector<T>::const_iterator it(obj.begin()),
         end(obj.end());
         it != end;
         ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T>
  temp_logger& operator<<(temp_logger const& left,
                          std::list<T> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename std::list<T>::const_iterator it(obj.begin()), end(obj.end());
         it != end; ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T>
  temp_logger& operator<<(temp_logger const& left,
                          std::set<T> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename std::set<T>::const_iterator it(obj.begin()), end(obj.end());
         it != end; ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T>
  temp_logger& operator<<(temp_logger const& left,
                          std::deque<T> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename std::deque<T>::const_iterator it(obj.begin()),
         end(obj.end());
         it != end;
         ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T, typename U>
  temp_logger& operator<<(temp_logger const& left,
                          std::map<T, U> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename std::map<T, U>::const_iterator it(obj.begin()),
         end(obj.end());
         it != end;
         ++it) {
      tmp << "{"
          << it->second
          << "}\n";
    }
    return (tmp);
  }

  template <typename T, typename U>
  temp_logger& operator<<(temp_logger const& left,
                          QHash<T, U> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename QHash<T, U>::const_iterator it(obj.begin()), end(obj.end());
         it != end; ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T>
  temp_logger& operator<<(temp_logger const& left,
                          QSet<T> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename QSet<T>::const_iterator it(obj.begin()), end(obj.end());
         it != end; ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T, typename U>
  temp_logger& operator<<(temp_logger const& left,
                          QMultiHash<T, U> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    for (typename QMultiHash<T, U>::const_iterator it(obj.begin()),
         end(obj.end());
         it != end;
         ++it) {
      tmp << "{"
          << *it
          << "}\n";
    }
    return (tmp);
  }

  template <typename T>
  temp_logger& operator<<(temp_logger const& left,
                          shared_ptr<T> const& obj) throw() {
    logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
    tmp << *obj;
    return (tmp);
  }
}

namespace notification {
  class data_logger {
  public:
    template <typename T>
    static void log_container(std::string const& container_name,
                              T const& container) {
      logging::debug(logging::low)
          << "Notification: Logging container called "
          << container_name;
      logging::debug(logging::low) << container;
    }
   };
}

CCB_END()

#endif //!CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH
