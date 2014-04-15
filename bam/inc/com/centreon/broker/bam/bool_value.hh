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

#ifndef CCB_BAM_BOOL_VALUE_HH
#  define CCB_BAM_BOOL_VALUE_HH

#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class bool_value bool_value.hh "com/centreon/broker/bam/bool_value.hh"
   *  @brief Computable boolean value.
   *
   *  This class abstracts a boolean value that can get computed.
   */
  class          bool_value : public computable {
  public:
    typedef misc::shared_ptr<bool_value> ptr;

                 bool_value();
                 bool_value(bool_value const& right);
    virtual      ~bool_value();
    bool_value&  operator=(bool_value const& right);
    virtual bool value_hard() = 0;
    virtual bool value_soft() = 0;
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_VALUE_HH
