/*
** Copyright 2015 Merethis
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

#ifndef CCB_IO_EVENT_INFO_HH
#  define CCB_IO_EVENT_INFO_HH

#  include <cstddef>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace                   mapping {
  class                     entry;
}

namespace                   io {
  // Forward declaration.
  class                     data;

  /**
   *  @class event_info event_info.hh "com/centreon/broker/io/event_info.hh"
   *  @brief Provide information about an event.
   *
   *  This class is mostly used to provide information about a mapped
   *  type. Various event manipulation engines (BBDO, SQL) use
   *  information provided by this class.
   */
  class                     event_info {
  public:
    struct                  event_operations {
      io::data* (*          constructor)();
    };

                            event_info(
                              std::string const& name = "",
                              event_operations const* ops = NULL,
                              mapping::entry const* entries = NULL,
                              std::string const& table = std::string());
                            event_info(event_info const& other);
                            ~event_info();
    event_info&             operator=(event_info const& other);
    mapping::entry const*   get_mapping() const;
    std::string const&      get_name() const;
    event_operations const& get_operations() const;
    std::string const&      get_table() const;

  private:
    mapping::entry const*   _mapping;
    std::string             _name;
    event_operations const* _ops;
    std::string             _table;
  };
}

CCB_END()

#endif // !CCB_IO_EVENT_INFO_HH
