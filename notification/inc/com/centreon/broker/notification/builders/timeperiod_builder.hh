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

#ifndef CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_BUILDER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/timeperiod.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class timeperiod_builder timeperiod_builder.hh "com/centreon/broker/notification/builders/timeperiod_builder.hh"
   *  @brief Timeperiod builder interface.
   *
   *  This interface define what methods timeperiod builders need to implement.
   */
  class           timeperiod_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual       ~timeperiod_builder() {}

    /**
     *  Add a timeperiod to the builder.
     *
     *  @param[in] id The id of the timeperiod.
     *  @param[in] tperiod The timeperiod.
     */
    virtual void  add_timeperiod(unsigned int id,
                                 timeperiod::ptr tperiod) {}
    virtual void  add_timeperiod_exception(unsigned int timeperiod_id,
                                           std::string const& days,
                                            std::string const& timerange) {}
    virtual void  add_timeperiod_exclude_relation(unsigned int timeperiod_id,
                                                  unsigned int exclude_id) {}
    virtual void  add_timeperiod_include_relation(unsigned int timeperiod_id,
                                                  unsigned int include_id) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_BUILDER_HH
