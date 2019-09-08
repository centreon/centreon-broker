/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH
#define CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH

#include <QHash>
#include <QSet>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/objects/command.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/dependency.hh"
#include "com/centreon/broker/notification/objects/node.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/objects/notification_method.hh"
#include "com/centreon/broker/notification/objects/notification_rule.hh"
#include "com/centreon/broker/time/timeperiod.hh"

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
                        notification::objects::dependency const&) throw();
temp_logger& operator<<(temp_logger const& left,
                        notification::objects::contact const&) throw();
temp_logger& operator<<(temp_logger const& left,
                        time::timeperiod const&) throw();
temp_logger& operator<<(temp_logger const& left,
                        notification::objects::node const&) throw();
temp_logger& operator<<(
    temp_logger const& left,
    notification::objects::notification_method const&) throw();
temp_logger& operator<<(
    temp_logger const& left,
    notification::objects::notification_rule const&) throw();

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::vector<T> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::vector<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::vector<std::shared_ptr<T> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::vector<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::list<T> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::list<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::list<std::shared_ptr<T> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::list<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::set<T> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::set<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::set<std::shared_ptr<T> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::set<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::deque<T> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::deque<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::deque<std::shared_ptr<T> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::deque<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T, typename U>
temp_logger& operator<<(temp_logger const& left,
                        std::map<T, U> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::map<T, U>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << it->second << "}\n";
  }
  return (tmp);
}

template <typename T, typename U>
temp_logger& operator<<(temp_logger const& left,
                        std::map<T, std::shared_ptr<U> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename std::map<T, U>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << it->second << "}\n";
  }
  return (tmp);
}

template <typename T, typename U>
temp_logger& operator<<(temp_logger const& left,
                        QHash<T, U> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename QHash<T, U>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T, typename U>
temp_logger& operator<<(temp_logger const& left,
                        QHash<T, std::shared_ptr<U> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename QHash<T, std::shared_ptr<U> >::const_iterator it(obj.begin()),
       end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left, QSet<T> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename QSet<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        QSet<std::shared_ptr<T> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename QSet<T>::const_iterator it(obj.begin()), end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T, typename U>
temp_logger& operator<<(temp_logger const& left,
                        QMultiHash<T, std::shared_ptr<U> > const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  for (typename QMultiHash<T, U>::const_iterator it(obj.begin()),
       end(obj.end());
       it != end; ++it) {
    tmp << "{" << *it << "}\n";
  }
  return (tmp);
}

template <typename T>
temp_logger& operator<<(temp_logger const& left,
                        std::shared_ptr<T> const& obj) throw() {
  logging::temp_logger& tmp = const_cast<logging::temp_logger&>(left);
  tmp << *obj;
  return (tmp);
}
}  // namespace logging

namespace notification {
class data_logger {
 public:
  template <typename T>
  static void log_container(std::string const& container_name,
                            T const& container) {
    logging::debug(logging::low)
        << "notification: logging container called " << container_name;
    logging::debug(logging::low) << container;
  }
};
}  // namespace notification

CCB_END()

#endif  //! CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH
