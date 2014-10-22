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
** <http:/www.gnu.org/licenses/>.
*/

#ifndef CCB_BAM_CONFIGURATION_APPLIER_TIMEPERIOD_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_TIMEPERIOD_HH

#  include <map>
#  include "com/centreon/broker/bam/time/timeperiod.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               bam {
  // Forward declaration.
  class                 stream;

  namespace             configuration {

    namespace           applier {
      /**
       *  @class timeperiod timeperiod.hh "com/centreon/broker/bam/configuration/applier/timeperiod.hh"
       *  @brief Apply the timeperiods configuration into the BAM engine.
       */
      class             timeperiod {
      public:
                        timeperiod();
                        timeperiod(timeperiod const& other);
        timeperiod&     operator=(timeperiod const& other);
        void            apply(configuration::state::timeperiods const& my_tps);

        time::timeperiod::ptr
                        get_timeperiod_by_id(unsigned int id);

      private:
        typedef std::map<unsigned int,
                         time::timeperiod::ptr> tp_map;
        tp_map          _timeperiods;

        time::timeperiod::ptr
          _create_timeperiod(configuration::timeperiod const& tp_cfg);
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_TIMEPERIOD_HH
