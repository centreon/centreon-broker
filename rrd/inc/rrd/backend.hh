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

#ifndef CCB_RRD_BACKEND_HH_
# define CCB_RRD_BACKEND_HH_

# include <QString>
# include <time.h>

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        rrd {
        /**
         *  @class backend backend.hh "rrd/backend.hh"
         *  @brief Generic access to RRD files.
         *
         *  Provide a unified access to RRD files. Files can be accessed
         *  either through librrd or with rrdcached.
         *
         *  @see rrd::lib
         *  @see rrd::cached
         */
        class          backend {
         public:
                       backend();
                       backend(backend const& b);
          virtual      ~backend();
          backend&     operator=(backend const& b);
          virtual void begin() = 0;
          virtual void close() = 0;
          virtual void commit() = 0;
          virtual void open(QString const& filename,
                         QString const& metric) = 0;
          virtual void open(QString const& filename,
                         QString const& metric,
                         unsigned int length,
                         time_t from,
                         time_t interval) = 0;
          virtual void update(time_t t, QString const& value) = 0;
        };
      }
    }
  }
}

#endif /* !CCB_RRD_BACKEND_HH_ */
