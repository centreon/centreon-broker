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

#ifndef CCB_NOTIFICATION_BUILDERS_GLOBAL_MACRO_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_GLOBAL_MACRO_BUILDER_HH

#include <QHash>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/macro_builder.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class global_macro_builder global_macro_builder.hh
 * "com/centreon/broker/notification/builders/global_macro_builder.hh"
 *  @brief Global macro builder.
 */
class global_macro_builder : public macro_builder {
 public:
  global_macro_builder(QHash<std::string, std::string>& global_macros,
                       int& date_format);

  virtual void add_global_macro(std::string const& macro_name,
                                std::string const& macro_value);
  virtual void add_date_format(int format);
  virtual void add_resource_macro(std::string const& macro_name,
                                  std::string const& macro_value);

 private:
  QHash<std::string, std::string>& _global_macros;
  int& _date_format;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_GLOBAL_MACRO_BUILDER_HH
