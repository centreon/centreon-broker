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

#ifndef CCB_NOTIFICATION_BUILDERS_GLOBAL_MACRO_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_GLOBAL_MACRO_BUILDER_HH

#  include <string>
#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/macro_builder.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class global_macro_builder global_macro_builder.hh "com/centreon/broker/notification/builders/global_macro_builder.hh"
   *  @brief Global macro builder.
   */
  class          global_macro_builder : public macro_builder {
  public:
                 global_macro_builder(
                   QHash<std::string, std::string>& global_macros,
                   int& date_format);

    virtual void add_global_macro(
                   std::string const& macro_name,
                   std::string const& macro_value);
    virtual void add_date_format(int format);
    virtual void add_resource_macro(
                   std::string const& macro_name,
                   std::string const& macro_value);

  private:
    QHash<std::string, std::string>&
                  _global_macros;
    int&          _date_format;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_GLOBAL_MACRO_BUILDER_HH
