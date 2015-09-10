/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_NEB_CUSTOM_VARIABLE_STATUS_HH
#  define CCB_NEB_CUSTOM_VARIABLE_STATUS_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace                   neb {
  /**
   *  @class custom_variable_status custom_variable_status.hh "com/centreon/broker/neb/custom_variable_status.hh"
   *  @brief Custom variable update.
   *
   *  The value of a custom variable has changed.
   */
  class                     custom_variable_status : public io::data {
  public:
                            custom_variable_status();
                            custom_variable_status(
                              custom_variable_status const& cvs);
    virtual                 ~custom_variable_status();
    custom_variable_status& operator=(custom_variable_status const& cvs);
    virtual unsigned int    type() const;

    unsigned int            host_id;
    bool                    modified;
    QString                 name;
    unsigned int            service_id;
    timestamp               update_time;
    QString                 value;

  private:
    void                    _internal_copy(custom_variable_status const& cvs);
  };
}

CCB_END()

#endif // !CCB_NEB_CUSTOM_VARIABLE_STATUS_HH
