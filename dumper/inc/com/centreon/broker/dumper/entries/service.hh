/*
** Copyright 2015 Centreon
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

#ifndef CCB_DUMPER_ENTRIES_SERVICE_HH
#  define CCB_DUMPER_ENTRIES_SERVICE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 dumper {
  namespace               entries {
    /**
     *  @class ba ba.hh "com/centreon/broker/dumper/entries/service.hh"
     *  @brief Service entry.
     *
     *  Some service row of the service configuration table.
     */
    class                 service : public io::data {
    public:
                          service();
                          service(service const& other);
                          ~service();
      service&            operator=(service const& other);
      bool                operator==(service const& other) const;
      bool                operator!=(service const& other) const;
      unsigned int        type() const;

      QString             description;
      bool                enable;
      unsigned int        host_id;
      unsigned int        service_id;
      unsigned int        poller_id;

    private:
      void                _internal_copy(service const& other);
    };
  }
}

CCB_END()

#endif // !CCB_DUMPER_ENTRIES_SERVICE_HH
