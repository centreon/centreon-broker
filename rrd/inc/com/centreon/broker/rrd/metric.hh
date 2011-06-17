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

#ifndef CCB_RRD_METRIC_HH_
# define CCB_RRD_METRIC_HH_

# include <QString>
# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          rrd {
        /**
         *  @class metric metric.hh "com/centreon/broker/rrd/metric.hh"
         *  @brief Metric information.
         *
         *  Metric information, used to update RRD files.
         */
        class            metric : public io::data {
         private:
          void           _internal_copy(metric const& m);

         public:
          time_t         ctime;
          time_t         interval;
          unsigned int   metric_id;
          QString        name;
          int            rrd_len;
          double         value;
                         metric();
                         metric(metric const& m);
                         ~metric();
          metric&        operator=(metric const& m);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_RRD_METRIC_HH_ */
