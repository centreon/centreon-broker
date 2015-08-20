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

#ifndef CCB_DUMPER_ENTRIES_BOOLEAN_HH
#  define CCB_DUMPER_ENTRIES_BOOLEAN_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba ba.hh "com/centreon/broker/dumper/entries/ba.hh"
     *  @brief Boolean rule entry.
     *
     *  Some boolean rule row of the boolean rule configuration table.
     */
    class                 boolean : public io::data {
    public:
                          boolean();
                          boolean(boolean const& other);
                          ~boolean();
      boolean&            operator=(boolean const& other);
      bool                operator==(boolean const& other) const;
      bool                operator!=(boolean const& other) const;
      unsigned int        type() const;

      bool                enable;
      unsigned int        poller_id;
      unsigned int        boolean_id;
      int                 bool_state;
      QString             name;
      QString             expression;
      QString             comment;

    private:
      void                _internal_copy(boolean const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_BOOLEAN_HH
