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

#ifndef CCB_RRD_CACHED_HH_
# define CCB_RRD_CACHED_HH_

# include <QIODevice>
# include <QScopedPointer>
# include <QString>
# include "backend.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   rrd {
        /**
         *  @class cached cached.hh "rrd/cached.hh"
         *  @brief Handle RRD file access through rrdcached.
         *
         *  Handle creation, deletion, tuning and update of an RRD file with
         *  rrdcached. The advantage of rrdcached is to have a batch mode that
         *  allows bulk operations.
         *
         *  @see begin()
         *  @see commit()
         */
        class     cached : public backend {
         private:
          bool    _batch;
          QString _filename;
          QString _metric;
          QScopedPointer<QIODevice>
                  _socket;
                  cached(cached const& c);
          cached& operator=(cached const& c);
          void    _send_to_cached(char const* command,
                    unsigned int size = 0);

         public:
                  cached();
                  ~cached();
          void    begin();
          void    close();
          void    commit();
          void    connect_local(QString const& name);
          void    connect_remote(QString const& address,
                    unsigned short port);
          void    open(QString const& filename,
                    QString const& metric);
          void    open(QString const& filename,
                    QString const& metric,
                    unsigned int length,
                    time_t from,
                    time_t interval);
          void    update(time_t t, QString const& value);
        };
      }
    }
  }
}

#endif /* !CCB_RRD_CACHED_HH_ */
