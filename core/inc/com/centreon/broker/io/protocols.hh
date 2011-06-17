/*
** Copyright 2011 Merethis
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

#ifndef CCB_IO_PROTOCOLS_HH_
# define CCB_IO_PROTOCOLS_HH_

# include <QMap>
# include <QSharedPointer>
# include <QString>
# include "com/centreon/broker/io/factory.hh"

namespace                           com {
  namespace                         centreon {
    namespace                       broker {
      namespace                     io {
        /**
         *  @class protocols protocols.hh "com/centreon/broker/io/protocols.hh"
         *  @brief Reference available protocols.
         *
         *  This class registers every available protocol that are used
         *  to build input or output objects.
         */
        class                       protocols {
         public:
          struct                    protocol {
            QSharedPointer<factory> endpntfactry;
            unsigned short          osi_from;
            unsigned short          osi_to;
          };

         private:
          QMap<QString, protocol>   _protocols;
                                    protocols();
                                    protocols(protocols const& p);
          protocols&                operator=(protocols const& p);

         public:
                                    ~protocols();
          QMap<QString, protocol>::const_iterator
                                    begin() const;
          QMap<QString, protocol>::const_iterator
                                    end() const;
          static protocols&         instance();
          void                      reg(QString const& name,
                                      factory const& fac,
                                      unsigned short osi_from,
                                      unsigned short osi_to);
          void                      unreg(QString const& name);
        };
      }
    }
  }
}

#endif /* !CCB_IO_PROTOCOLS_HH_ */
