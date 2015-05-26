/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_BBDO_ACCEPTOR_HH
#  define CCB_BBDO_ACCEPTOR_HH

#  include <ctime>
#  include <list>
#  include <QMutex>
#  include <QString>
#  include <QThread>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace         processing {
  class           thread;
}

namespace         bbdo {
  // Forward declaration.
  class           stream;

  /**
   *  @class acceptor acceptor.hh "com/centreon/broker/bbdo/acceptor.hh"
   *  @brief BBDO acceptor.
   *
   *  Accept incoming BBDO connections.
   */
  class           acceptor : public io::endpoint {
  public:
                  acceptor(
                    QString const& name,
                    bool is_out,
                    bool negociate,
                    QString const& extensions,
                    time_t timeout,
                    bool one_peer_retention_mode = false,
                    bool coarse = false);
                  acceptor(acceptor const& other);
                  ~acceptor();
    acceptor&     operator=(acceptor const& other);
    io::endpoint* clone() const;
    void          close();
    misc::shared_ptr<io::stream>
                  open();
    void          stats(io::properties& tree);

  private:
    unsigned int  _negociate_features(
                     misc::shared_ptr<io::stream> stream,
                     misc::shared_ptr<bbdo::stream> my_bbdo);
    void          _open(misc::shared_ptr<io::stream> stream);

    bool          _coarse;
    QString       _extensions;
    bool          _is_out;
    QString       _name;
    bool          _negociate;
    bool          _one_peer_retention_mode;
    time_t        _timeout;
  };
}

CCB_END()

#endif // !CCB_BBDO_ACCEPTOR_HH
