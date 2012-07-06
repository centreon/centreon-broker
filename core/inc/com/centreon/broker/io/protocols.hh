/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_IO_PROTOCOLS_HH
#  define CCB_IO_PROTOCOLS_HH

#  include <QMap>
#  include <QString>
#  include "com/centreon/broker/io/factory.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                       io {
  /**
   *  @class protocols protocols.hh "com/centreon/broker/io/protocols.hh"
   *  @brief Reference available protocols.
   *
   *  This class registers every available protocol that are used
   *  to build input or output objects.
   */
  class                         protocols {
  public:
    struct                      protocol {
      misc::shared_ptr<factory> endpntfactry;
      unsigned short            osi_from;
      unsigned short            osi_to;
    };

                                ~protocols();
    QMap<QString, protocol>::const_iterator
                                begin() const;
    QMap<QString, protocol>::const_iterator
                                end() const;
    static protocols&           instance();
    static void                 load();
    void                        reg(
                                  QString const& name,
                                  factory const& fac,
                                  unsigned short osi_from,
                                  unsigned short osi_to);
    static void                 unload();
    void                        unreg(QString const& name);

  private:
                                protocols();
                                protocols(protocols const& p);
    protocols&                  operator=(protocols const& p);

    QMap<QString, protocol>     _protocols;
  };
}

CCB_END()

#endif // !CCB_IO_PROTOCOLS_HH
