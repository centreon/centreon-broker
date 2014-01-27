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

#ifndef CCB_STATS_CONFIG_HH
#  define CCB_STATS_CONFIG_HH

#  include <list>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                    stats {
  // Forward declaration.
  class                      metric;

  /**
   *  @class config config.hh "com/centreon/broker/stats/config.hh"
   *  @brief Stats module configuration.
   *
   *  Holds the statistics module configuration.
   */
  class                      config {
  public:
                             config();
                             config(config const& right);
                             ~config();
    config&                  operator=(config const& right);
    std::string const&       get_dumper_tag() const throw ();
    std::string const&       get_fifo() const throw ();
    unsigned int             get_interval() const throw ();
    std::list<metric>&       metrics() throw ();
    std::list<metric> const& metrics() const throw ();
    void                     set_dumper_tag(std::string const& tag);
    void                     set_fifo(std::string const& fifo);
    void                     set_interval(
                               unsigned int interval) throw ();

  private:
    std::string              _dumper_tag;
    std::string              _fifo;
    unsigned int             _interval;
    std::list<metric>        _metrics;
  };
}

CCB_END()

#endif // !CCB_STATS_CONFIG_HH
