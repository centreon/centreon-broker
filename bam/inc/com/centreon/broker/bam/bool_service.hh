/*
** Copyright 2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_BAM_BOOL_SERVICE_HH
#  define CCB_BAM_BOOL_SERVICE_HH

#  include "com/centreon/broker/bam/bool_value.hh"
#  include "com/centreon/broker/bam/service_listener.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

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

                  bool_service();
                  bool_service(bool_service const& right);
                  ~bool_service();
    bool_service& operator=(bool_service const& right);
    bool          child_has_update(
                    computable* child,
                    io::stream* visitor = NULL);
    unsigned int  get_host_id() const;
    unsigned int  get_service_id() const;
    void          set_expected_state(short expected);
    void          set_host_id(unsigned int host_id);
    void          set_service_id(unsigned int service_id);
    void          set_value_if_state_match(bool value);
    void          service_update(
                    misc::shared_ptr<neb::service_status> const& status,
                    io::stream* visitor = NULL);
    bool          value_hard();
    bool          value_soft();
    bool          state_known() const;

  private:
    void          _internal_copy(bool_service const& right);

    unsigned int  _host_id;
    unsigned int  _service_id;
    short         _state_expected;
    short         _state_hard;
    short         _state_soft;
    bool          _value_if_state_match;
    bool          _state_known;
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_SERVICE_HH
