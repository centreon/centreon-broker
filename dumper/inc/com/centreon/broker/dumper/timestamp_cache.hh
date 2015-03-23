/*
** Copyright 2013 Merethis
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

#ifndef CCB_DUMPER_TIMESTAMP_CACHE_HH
#  define CCB_DUMPER_TIMESTAMP_CACHE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace        dumper {
  /**
   *  @class timestamp_cache timestamp_cache.hh "com/centreon/broker/dumper/timestamp_cache.hh"
   *  @brief Cache the last modified time of a file.
   *
   *  This is used to cache this information in the persistant cache.
   */
  class          timestamp_cache : public io::data {
  public:
                 timestamp_cache();
                 timestamp_cache(timestamp_cache const& right);
                 ~timestamp_cache();
    timestamp_cache&
                 operator=(timestamp_cache const& right);
    unsigned int type() const;
    static unsigned int
                 static_type();

    QString      filename;
    timestamp    last_modified;

    static mapping::entry const
                    entries[];
    static io::event_info::event_operations const
                    operations;

  private:
    void         _internal_copy(timestamp_cache const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_TIMESTAMP_CACHE_HH
