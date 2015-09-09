/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_IO_TEMPORARY_HH
#  define CCB_IO_TEMPORARY_HH

#  include <QMutex>
#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             io {
  /**
   *  @class temporary temporary.hh "com/centreon/broker/io/temporary.hh"
   *  @brief Temporary builder.
   *
   *  Build temporary endpoint according to some configuration.
   */
  class               temporary {
  public:
    misc::shared_ptr<io::stream>
                      create(QString const& name);
    static temporary& instance();
    static void       load();
    void              set(misc::shared_ptr<io::endpoint> endp);
    static void       unload();

  private:
                      temporary();
                      temporary(temporary const& right);
                      ~temporary();
    temporary&        operator=(temporary const& right);

    QMutex            _lock;
    misc::shared_ptr<io::endpoint>
                      _endp;
  };
}

CCB_END()

#endif // !CCB_IO_TEMPORARY_HH
