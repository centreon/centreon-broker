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

#ifndef CCB_RRD_OUTPUT_HH_
# define CCB_RRD_OUTPUT_HH_

# include <QScopedPointer>
# include <QString>
# include "io/stream.hh"
# include "rrd/backend.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   rrd {
        /**
         *  @class output output.hh "rrd/output.hh"
         *  @brief RRD output class.
         *
         *  Write RRD files.
         */
        class     output : public com::centreon::broker::io::stream {
         private:
	  QScopedPointer<backend>
                  _backend;
          QString _rrd_path;

         public:
                  output(QString const& rrd_path);
                  output(output const& o);
                  ~output();
          output& operator=(output const& o);
          QSharedPointer<com::centreon::broker::io::data>
                  read();
          void    write(QSharedPointer<com::centreon::broker::io::data> d);
        };
      }
    }
  }
}

#endif /* !CCB_RRD_OUTPUT_HH_ */
