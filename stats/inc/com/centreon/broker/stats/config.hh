/*
** Copyright 2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_STATS_CONFIG_HH
#  define CCB_STATS_CONFIG_HH

#  include <list>
#  include <string>
#  include <vector>
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
    enum                     fifo_type {
                             plain_text,
                             json
    };
    typedef std::vector<std::pair<std::string, fifo_type> >
                             fifo_list;

                             config();
                             config(config const& right);
                             ~config();
    config&                  operator=(config const& right);
    std::string const&       get_dumper_tag() const throw ();
    fifo_list const&         get_fifo() const throw ();
    unsigned int             get_interval() const throw ();
    std::list<metric>&       metrics() throw ();
    std::list<metric> const& metrics() const throw ();
    void                     set_dumper_tag(std::string const& tag);
    void                     add_fifo(std::string const& fifo, fifo_type type);
    void                     set_interval(
                               unsigned int interval) throw ();

  private:
    std::string              _dumper_tag;
    fifo_list                _fifos;
    unsigned int             _interval;
    std::list<metric>        _metrics;
  };
}

CCB_END()

#endif // !CCB_STATS_CONFIG_HH
