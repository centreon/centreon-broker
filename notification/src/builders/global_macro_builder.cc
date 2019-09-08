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

#include "com/centreon/broker/notification/builders/global_macro_builder.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

using namespace com::centreon::broker::notification;

global_macro_builder::global_macro_builder(
    QHash<std::string, std::string>& global_macros,
    int& date_format)
    : _global_macros(global_macros), _date_format(date_format) {}

/**
 *  Add a macro to the builder.
 *
 *  @param[in] macro_name   The macro name.
 *  @param[in] macro_value  The macro value.
 */
void global_macro_builder::add_global_macro(std::string const& macro_name,
                                            std::string const& macro_value) {
  _global_macros.insert(macro_name, macro_value);
}

/**
 *  Add a date format to the builder.
 *
 *  @param[in] format  The date format to add.
 */
void global_macro_builder::add_date_format(int format) {
  _date_format = format;
}

/**
 *  Add a resource macro to the builder
 *
 *  @param[in] macro_name   The macro name.
 *  @param[in] macro_value  The macro value.
 */
void global_macro_builder::add_resource_macro(std::string const& macro_name,
                                              std::string const& macro_value) {
  _global_macros.insert(macro_name, macro_value);
}
