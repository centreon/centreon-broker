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

#ifndef CCB_NOTIFICATION_BUILDERS_MACRO_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_MACRO_BUILDER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class macro_builder macro_builder.hh "com/centreon/broker/notification/builders/macro_builder.hh"
   *  @brief Macro builder interface.
   *
   *  This interface define what methods macro builders need to implement.
   */
  class          macro_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual      ~macro_builder() {}

    /**
     *  Add a global macro to the builder.
     *
     *  @param[in] macro_name   The macro name.
     *  @param[in] macro_value  The macro value.
     */
    virtual void add_global_macro(
                   std::string const& macro_name,
                   std::string const& macro_value) {
      (void)macro_name;
      (void)macro_value;
    }

    /**
     *  Add the date format of the macros to the builder.
     *
     *  @param[in] format  The date format.
     */
    virtual void add_date_format(int format) {
      (void)format;
    }

    /**
     *  Add a resource macro to the builder.
     *
     *  @param[in] macro_name   The macro name.
     *  @param[in] macro_value  The macro value.
     */
    virtual void add_resource_macro(
                   std::string const& macro_name,
                   std::string const& macro_value) {
      (void)macro_name;
      (void)macro_value;
    }
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_NODE_BUILDER_HH
