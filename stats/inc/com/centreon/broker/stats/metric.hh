/*
** Copyright 2013 Merethis
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

#ifndef CCB_STATS_METRIC_HH
#  define CCB_STATS_METRIC_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace              stats {
  /**
   *  @class metric metric.hh "com/centreon/broker/stats/metric.hh"
   *  @brief Configuration of a metric.
   *
   *  Holds parameters of a statistic metric.
   */
  class                metric {
  public:
                       metric();
                       metric(metric const& right);
                       ~metric();
    metric&            operator=(metric const& right);
    unsigned int       get_host_id() const throw ();
    std::string const& get_name() const throw ();
    unsigned int       get_service_id() const throw ();
    void               set_host_id(unsigned int host_id) throw ();
    void               set_name(std::string const& name);
    void               set_service_id(unsigned int service_id) throw ();

  private:
    unsigned int       _host_id;
    std::string        _name;
    unsigned int       _service_id;
  };
}

CCB_END()

#endif // !CCB_STATS_METRIC_HH
