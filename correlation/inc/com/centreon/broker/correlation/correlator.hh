/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_CORRELATION_CORRELATOR_HH_
# define CCB_CORRELATION_CORRELATOR_HH_

# include <map>
# include <QList>
# include <QScopedPointer>
# include "com/centreon/broker/correlation/node.hh"
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/neb/host_status.hh"
# include "com/centreon/broker/neb/log_entry.hh"

namespace                          com {
  namespace                        centreon {
    namespace                      broker {
      namespace                    correlation {
        /**
         *  @class correlator correlator.hh "correlation/correlator.hh"
         *  @brief Create event correlation.
         *
         *  This class aggregate data from multiple events and generates
         *  according issues which are used to group those events.
         */
        class                      correlator : public io::stream {
         private:
          QList<QSharedPointer<io::data> >
                                   _events;
          std::map<unsigned int, node>
                                   _hosts;
          std::map<std::pair<unsigned int, unsigned int>, node>
                                   _services;
          void                     _correlate_host_service_status(QSharedPointer<io::data> e,
                                     bool is_host);
          void                     _correlate_host_status(QSharedPointer<io::data> e);
          void                     _correlate_log(QSharedPointer<io::data> e);
          void                     _correlate_service_status(QSharedPointer<io::data> e);
          QSharedPointer<issue>    _find_related_issue(node& n);
          void                     _internal_copy(correlator const& c);

         public:
                                   correlator();
                                   correlator(correlator const& c);
                                   ~correlator();
          correlator&              operator=(correlator const& c);
          void                     load(char const* correlation_file);
          QSharedPointer<io::data> read();
          void                     write(QSharedPointer<io::data> e);
        };
      }
    }
  }
}

#endif /* !CCB_CORRELATION_CORRELATOR_HH_ */
