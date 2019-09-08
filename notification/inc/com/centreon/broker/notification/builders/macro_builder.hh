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

#ifndef CCB_NOTIFICATION_BUILDERS_MACRO_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_MACRO_BUILDER_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class macro_builder macro_builder.hh
 * "com/centreon/broker/notification/builders/macro_builder.hh"
 *  @brief Macro builder interface.
 *
 *  This interface define what methods macro builders need to implement.
 */
class macro_builder {
 public:
  /**
   *  Virtual destructor.
   */
  virtual ~macro_builder() {}

  /**
   *  Add a global macro to the builder.
   *
   *  @param[in] macro_name   The macro name.
   *  @param[in] macro_value  The macro value.
   */
  virtual void add_global_macro(std::string const& macro_name,
                                std::string const& macro_value) {
    (void)macro_name;
    (void)macro_value;
  }

  /**
   *  Add the date format of the macros to the builder.
   *
   *  @param[in] format  The date format.
   */
  virtual void add_date_format(int format) { (void)format; }

  /**
   *  Add a resource macro to the builder.
   *
   *  @param[in] macro_name   The macro name.
   *  @param[in] macro_value  The macro value.
   */
  virtual void add_resource_macro(std::string const& macro_name,
                                  std::string const& macro_value) {
    (void)macro_name;
    (void)macro_value;
  }
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_NODE_BUILDER_HH
