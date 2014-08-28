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

#ifndef CCB_BAM_META_SERVICE_HH
#  define CCB_BAM_META_SERVICE_HH

#  include <string>
#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/bam/metric_listener.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace            storage {
  class              metric;
}

namespace            bam {
  /**
   *  @class meta_service meta_service.hh "com/centreon/broker/bam/meta_service.hh"
   *  @brief Compute meta-service.
   *
   *  This is the class computing meta-services (aggregation of service
   *  metrics) with some computation method (SUM, AVERAGE, MIN, MAX,
   *  ...).
   */
  class              meta_service : public computable,
                                    public metric_listener {
  public:
    enum             computation_type {
      average = 1,
      min,
      max,
      sum
    };

                     meta_service();
                     meta_service(meta_service const& other);
                     ~meta_service();
    meta_service&    operator=(meta_service const& other);
    void             add_metric(unsigned int metric_id);
    void             child_has_update(
                       computable* child,
                       stream* visitor = NULL);
    unsigned int     get_id() const;
    void             metric_update(
                       misc::shared_ptr<storage::metric> const& m,
                       stream* visitor = NULL);
    void             remove_metric(unsigned int metric_id);
    void             recompute();
    void             set_computation(computation_type type);
    void             set_id(unsigned int id);
    void             set_level_critical(double level);
    void             set_level_warning(double level);
    void             visit(stream* visitor);

  private:
    static int const _recompute_limit = 100;

    void             _internal_copy(meta_service const& other);
    void             _recompute_partial(
                       double new_value,
                       double old_value);

    computation_type _computation;
    unsigned int     _id;
    double           _level_critical;
    double           _level_warning;
    umap<unsigned int, double>
                     _metrics;
    int              _recompute_count;
    double           _value;
  };
}

CCB_END()

#endif // !CCB_BAM_META_SERVICE_HH
