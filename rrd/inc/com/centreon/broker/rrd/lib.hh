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

#ifndef CCB_RRD_LIB_HH_
# define CCB_RRD_LIB_HH_

# include "com/centreon/broker/rrd/backend.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          rrd {
        /**
         *  @class lib lib.hh "com/centreon/broker/rrd/lib.hh"
         *  @brief Handle RRD file access through librrd.
         *
         *  Handle creation, deletion, tuning and update of an RRD file
         *  with librrd.
         */
        class            lib : public backend {
         private:
          QString        _filename;
          QString        _metric; // XXX : is it necessary ?

         public:
                         lib();
                         lib(lib const& l);
                         ~lib();
          lib&           operator=(lib const& l);
          void           begin();
          void           close();
          void           commit();
	  static QString normalize_metric_name(QString const& metric);
          void           open(QString const& filename,
                           QString const& metric);
          void           open(QString const& filename,
                           QString const& metric,
                           unsigned int length,
                           time_t from,
                           time_t interval);
          void           update(time_t t,
                           QString const& value);
        };
      }
    }
  }
}

#endif /* !CCB_RRD_LIB_HH_ */
