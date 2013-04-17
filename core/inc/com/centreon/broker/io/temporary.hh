/*
** Copyright 2011-2013 Merethis
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
