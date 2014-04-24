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

#ifndef CCB_BAM_BOOL_BINARY_OPERATOR_HH
#  define CCB_BAM_BOOL_BINARY_OPERATOR_HH

#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 bam {
  /**
   *  @class bool_binary_operator bool_binary_operator.hh "com/centreon/broker/bam/bool_binary_operator.hh"
   *  @brief Abstracts a binary boolean operator (AND, OR, XOR).
   *
   *  Provides common methods to binary boolean operators.
   */
  class                   bool_binary_operator : public bool_value {
  public:
    typedef misc::shared_ptr<bool_binary_operator> ptr;

                          bool_binary_operator();
                          bool_binary_operator(
                            bool_binary_operator const& right);
    virtual               ~bool_binary_operator();
    bool_binary_operator& operator=(bool_binary_operator const& right);
    void                  child_has_update(
                            computable* child,
                            stream* visitor = NULL);
    void                  set_left(misc::shared_ptr<bool_value>& left);
    void                  set_right(misc::shared_ptr<bool_value>& right);

  protected:
    misc::shared_ptr<bool_value>
                          _left;
    bool                  _left_hard;
    bool                  _left_soft;
    misc::shared_ptr<bool_value>
                          _right;
    bool                  _right_hard;
    bool                  _right_soft;

  private:
    void                  _internal_copy(
                            bool_binary_operator const& right);
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_BINARY_OPERATOR_HH
