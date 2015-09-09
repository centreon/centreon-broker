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

#ifndef CCB_BAM_BOOL_STATUS_HH
#  define CCB_BAM_BOOL_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class bool_status bool_status.hh "com/centreon/broker/bam/bool_status.hh"
   *  @brief Update status of a boolean expression.
   *
   *  Update the status of a boolean expression, used to update the
   *  mod_bam_boolean table.
   */
  class          bool_status : public io::data {
  public:
                 bool_status();
                 bool_status(bool_status const& other);
                 ~bool_status();
    bool_status& operator=(bool_status const& other);
    unsigned int type() const;

    unsigned int bool_id;
    bool         state;

  private:
    void         _internal_copy(bool_status const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_STATUS_HH
