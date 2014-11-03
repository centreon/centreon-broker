
/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_BAM_TIME_TIME_INFO_HH
#  define CCB_BAM_TIME_TIME_INFO_HH

#  include <ctime>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             bam {
  namespace           time {
    /**
     *  @class time_info time_info.hh "com/centreon/broker/bam/time/time_info.hh"
     *  @brief  Internal struct time information.
     */
    struct time_info {
      time_t midnight;
      time_t preferred_time;
      tm     preftime;
    };
  }
}

CCB_END()

#endif // !CCB_BAM_TIME_TIME_INFO_HH
