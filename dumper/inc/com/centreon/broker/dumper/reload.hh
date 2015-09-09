/*
** Copyright 2013 Centreon
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

#ifndef CCB_DUMPER_RELOAD_HH
#  define CCB_DUMPER_RELOAD_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class reload reload.hh "com/centreon/broker/dumper/reload.hh"
   *  @brief Event for an asked reload.
   *
   *  Reload asked.
   */
  class                 reload : public io::data {
  public:
                        reload();
                        reload(reload const& right);
                        ~reload();
    reload&             operator=(reload const& right);
    unsigned int        type() const;

    QString             tag;

  private:
    void                _internal_copy(reload const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_RELOAD_HH
