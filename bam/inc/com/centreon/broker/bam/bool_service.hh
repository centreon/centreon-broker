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

#ifndef CCB_BAM_BOOL_SERVICE_HH
#  define CCB_BAM_BOOL_SERVICE_HH

#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/bam/service_listener.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/defines.hh"
CCB_BEGIN()

namespace         bam {
  /**
   *  @class bool_service bool_service.hh "com/centreon/broker/bam/bool_service.hh"
   *  @brief Evaluation of a service state.
   *
   *  This class compares the state of a service to compute a boolean
   *  value.
   */
  class           bool_service : public bool_value,
                                 public service_listener {
  public:
    typedef misc::shared_ptr<bool_service> ptr;

                  bool_service(unsigned int host_id,
                               unsigned int service_id );
                  bool_service(bool_service const& right);
                  ~bool_service();
    bool_service& operator=(bool_service const& right);
    void          child_has_update( computable::ptr& child);
    void          set_expected_state( e_service_state expected);
    void          set_host_id(unsigned int host_id);
    void          set_service_id(unsigned int service_id);
    void          set_value_if_state_match(bool value);
    void          service_update(
                    misc::shared_ptr<neb::service_status> const& status);
    bool          value_hard();
    bool          value_soft();

  private:
    void          _internal_copy(bool_service const& right);

    unsigned int     _host_id;
    unsigned int     _service_id;
    e_service_state  _state_expected;
    e_service_state  _state_hard;
    e_service_state  _state_soft;
    bool             _value_if_state_match;
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_SERVICE_HH
