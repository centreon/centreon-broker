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

#ifndef CCB_DUMPER_ENTRIES_ORGANIZATION_HH
#  define CCB_DUMPER_ENTRIES_ORGANIZATION_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class organization organization.hh "com/centreon/broker/dumper/entries/organization.hh"
     *  @brief Organization entry.
     *
     *  An organization as defined by Centreon software.
     */
    class                 organization : public io::data {
    public:
                          organization();
                          organization(organization const& other);
                          ~organization();
      organization&       operator=(organization const& other);
      bool                operator==(organization const& other) const;
      bool                operator!=(organization const& other) const;
      unsigned int        type() const;
      static unsigned int static_type();

      bool                enable;
      QString             name;
      unsigned int        organization_id;
      QString             shortname;

      static mapping::entry const
                          entries[];
      static io::event_info::event_operations const
                          operations;

    private:
      void                _internal_copy(organization const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_ORGANIZATION_HH
