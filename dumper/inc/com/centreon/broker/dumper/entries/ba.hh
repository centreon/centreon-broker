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

#ifndef CCB_DUMPER_ENTRIES_BA_HH
#  define CCB_DUMPER_ENTRIES_BA_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba ba.hh "com/centreon/broker/dumper/entries/ba.hh"
     *  @brief BA entry.
     *
     *  Some BA row of the BA configuration table.
     */
    class                 ba : public io::data {
    public:
                          ba();
                          ba(ba const& other);
                          ~ba();
      ba&                 operator=(ba const& other);
      bool                operator==(ba const& other) const;
      bool                operator!=(ba const& other) const;
      unsigned int        type() const;
      static unsigned int static_type();

      bool                enable;
      unsigned int        poller_id;
      unsigned int        ba_id;
      QString             description;
      double              level_critical;
      double              level_warning;
      QString             name;

    private:
      void                _internal_copy(ba const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_BA_HH
