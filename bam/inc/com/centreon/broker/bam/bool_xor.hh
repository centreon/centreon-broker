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

#ifndef CCB_BAM_BOOL_XOR_HH
#  define CCB_BAM_BOOL_XOR_HH

#  include "com/centreon/broker/bam/bool_binary_operator.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     bam {
  /**
   *  @class bool_xor bool_xor.hh "com/centreon/broker/bam/bool_xor.hh"
   *  @brief XOR operator.
   *
   *  In the context of a KPI computation, bool_xor represents a logical
   *  XOR between two bool_value.
   */
  class       bool_xor : public bool_binary_operator {
  public:
              bool_xor();
              bool_xor(bool_xor const& right);
              ~bool_xor();
    bool_xor& operator=(bool_xor const& right);
    bool      value_hard();
    bool      value_soft();
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_XOR_HH
