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

#ifndef CCB_BAM_BOOL_AND_HH
#  define CCB_BAM_BOOL_AND_HH

#  include "com/centreon/broker/bam/bool_binary_operator.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     bam {
  /**
   *  @class bool_and bool_and.hh "com/centreon/broker/bam/bool_and.hh"
   *  @brief AND operator.
   *
   *  In the context of a KPI computation, bool_and represents a logical
   *  AND between two bool_value.
   */
  class       bool_and : public bool_binary_operator {
  public:
              bool_and();
              bool_and(bool_and const& right);
              ~bool_and();
    bool_and& operator=(bool_and const& right);
    bool      value_hard();
    bool      value_soft();
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_AND_HH
