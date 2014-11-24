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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/builders/global_macro_builder.hh"

using namespace com::centreon::broker::notification;

global_macro_builder::global_macro_builder(
                        QHash<std::string, std::string>& global_macros,
                        int& date_format)
  : _global_macros(global_macros),
    _date_format(date_format) {}

/**
*  Add a macro to the builder.
*
*  @param[in] macro_name   The macro name.
*  @param[in] macro_value  The macro value.
*/
void global_macro_builder::add_global_macro(
                             std::string const& macro_name,
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
