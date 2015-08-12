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

#ifndef CCB_DUMPER_ENTRIES_SERVICE_HH
#  define CCB_DUMPER_ENTRIES_SERVICE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba ba.hh "com/centreon/broker/dumper/entries/service.hh"
     *  @brief Service entry.
     *
     *  Some service row of the service configuration table.
     */
    class                 service : public io::data {
    public:
                          service();
                          service(service const& other);
                          ~service();
      service&            operator=(service const& other);
      bool                operator==(service const& other) const;
      bool                operator!=(service const& other) const;
      unsigned int        type() const;

      QString             description;
      bool                enable;
      unsigned int        host_id;
      unsigned int        service_id;
      unsigned int        poller_id;

    private:
      void                _internal_copy(service const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_SERVICE_HH
