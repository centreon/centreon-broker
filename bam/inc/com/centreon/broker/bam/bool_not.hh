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

#ifndef CCB_BAM_BOOL_NOT_HH
#  define CCB_BAM_BOOL_NOT_HH

#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     bam {
  // Forward declaration.
  class       bool_value;

  /**
   *  @class bool_not bool_not.hh "com/centreon/broker/bam/bool_not.hh"
   *  @brief NOT boolean operator.
   *
   *  In the context of a KPI computation, bool_not represents a logical
   *  NOT on a bool_value.
   */
  class       bool_not : public bool_value {
  public:
              bool_not(bool_value::ptr val = bool_value::ptr());
              bool_not(bool_not const& right);
              ~bool_not();
    bool_not& operator=(bool_not const& right);
    bool      child_has_update(
                computable* child,
                io::stream* visitor = NULL);
    void      set_value(misc::shared_ptr<bool_value>& value);
    bool      value_hard();
    bool      value_soft();
    bool      state_known() const;

  private:
    void      _internal_copy(bool_not const& right);

    bool_value::ptr
              _value;
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_NOT_HH
