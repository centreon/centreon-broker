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

#ifndef CCB_DUMPER_ENTRIES_BA_TYPE_HH
#  define CCB_DUMPER_ENTRIES_BA_TYPE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba_type ba_type.hh "com/centreon/broker/dumper/entries/ba_type.hh"
     *  @brief BA type.
     *
     *  BA type, used by UI.
     */
    class                 ba_type : public io::data {
    public:
                          ba_type();
                          ba_type(ba_type const& other);
                          ~ba_type();
      ba_type&            operator=(ba_type const& other);
      bool                operator==(ba_type const& other) const;
      bool                operator!=(ba_type const& other) const;
      unsigned int        type() const;
      static unsigned int static_type();

      unsigned int        ba_type_id;
      QString             description;
      QString             name;
      QString             slug;

      static mapping::entry const
                          entries[];
      static io::event_info::event_operations const
                          operations;

    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_BA_TYPE_HH
