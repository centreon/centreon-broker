/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_METRIC_LISTENER_HH
#  define CCB_BAM_METRIC_LISTENER_HH

#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace             storage {
  class               metric;
}

namespace             bam {
  /**
   *  @class metric_listener metric_listener.hh "com/centreon/broker/bam/metric_listener.hh"
   *  @brief Listen to metric state change.
   *
   *  This interface is used by classes wishing to listen to metric
   *  state change.
   */
  class               metric_listener {
  public:
                      metric_listener();
                      metric_listener(metric_listener const& other);
    virtual           ~metric_listener();
    metric_listener&  operator=(metric_listener const& other);

    /**
     *  Notify of metric update.
     *
     *  @param[in] status Metric status.
     */
    virtual void      metric_update(
                        misc::shared_ptr<storage::metric> const& m,
                        io::stream* visitor = NULL) = 0;
  };
}

CCB_END()

#endif // !CCB_BAM_METRIC_LISTENER_HH
