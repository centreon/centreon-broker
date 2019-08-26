/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_DIMENSION_TIMEPERIOD_HH
#  define CCB_BAM_DIMENSION_TIMEPERIOD_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class dimension_timeperiod dimension_timeperiod.hh "com/centreon/broker/bam/dimension_timeperiod.hh"
   *  @brief Timeperiod dimension
   *
   */
  class            dimension_timeperiod : public io::data {
  public:
                   dimension_timeperiod();
                   ~dimension_timeperiod();
                   dimension_timeperiod(dimension_timeperiod const&);
    dimension_timeperiod&
                   operator=(dimension_timeperiod const&);
    bool           operator==(dimension_timeperiod const& other) const;
    unsigned int   type() const;
    static unsigned int
                   static_type();

    unsigned int   id;
    std::string        name;
    std::string        monday;
    std::string        tuesday;
    std::string        wednesday;
    std::string        thursday;
    std::string        friday;
    std::string        saturday;
    std::string        sunday;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(dimension_timeperiod const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_TIMEPERIOD_HH
