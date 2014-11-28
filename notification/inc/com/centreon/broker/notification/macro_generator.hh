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

#ifndef CCB_NOTIFICATION_MACRO_GENERATOR_HH
#  define CCB_NOTIFICATION_MACRO_GENERATOR_HH

#  include <QHash>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/state.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/node_cache.hh"
#  include "com/centreon/broker/notification/macro_context.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class macro_generator macro_generator.hh "com/centreon/broker/notification/macro_generator.hh"
   *  @brief Generates the macros.
   */
  class          macro_generator {
  public:
    typedef QHash<std::string, std::string>
                 macro_container;
                 macro_generator();

    void         generate(macro_container& container,
                   objects::node_id id,
                   objects::contact const& cnt,
                   state const& st,
                   node_cache const& cache) const;

  private:
    static bool  _get_global_macros(
                   std::string const& macro_name,
                   state const& st,
                   std::string& result);
    bool         _get_x_macros(std::string const& macro_name,
                   objects::node_id id,
                   state const& st,
                   node_cache const& cache,
                   objects::contact const& cnt,
                   std::string& result) const;

    typedef QHash<std::string, std::string (*)(macro_context const&)>
                  x_macro_map;
    static x_macro_map
                  _map;
    static void   _fill_x_macro_map(x_macro_map& map);
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_MACRO_GENERATOR_HH
