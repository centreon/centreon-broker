/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NEB_COMPOSED_TIMEPERIOD_BUILDER_HH
#  define CCB_NEB_COMPOSED_TIMEPERIOD_BUILDER_HH

#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/time/timeperiod.hh"
#  include "com/centreon/broker/neb/timeperiod_builder.hh"
#  include "com/centreon/broker/neb/composed_builder.hh"

CCB_BEGIN()

namespace         neb {
  /**
   *  @class composed_timeperiod_builder composed_timeperiod_builder.hh "com/centreon/broker/neb/composed_timeperiod_builder.hh"
   *  @brief Composed timeperiod builder.
   *
   *  This class forward its method call to several other builders.
   */
  class           composed_timeperiod_builder
                    : public composed_builder<timeperiod_builder> {
  public:
                  composed_timeperiod_builder();
    virtual       ~composed_timeperiod_builder() {}

    virtual void  add_timeperiod(
                    unsigned int id,
                    time::timeperiod::ptr tperiod);
    virtual void  add_timeperiod_exception(
                    unsigned int timeperiod_id,
                    std::string const& days,
                    std::string const& timerange);
    virtual void  add_timeperiod_exclude_relation(
                    unsigned int timeperiod_id,
                    unsigned int exclude_id);
    virtual void  add_timeperiod_include_relation(
                    unsigned int timeperiod_id,
                    unsigned int include_id);
  };

}

CCB_END()

#endif // !CCB_NEB_COMPOSED_TIMEPERIOD_BUILDER_HH
