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

#ifndef CCB_DUMPER_REMOVE_HH
#  define CCB_DUMPER_REMOVE_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace        dumper {
  /**
   *  @class remove remove.hh "com/centreon/broker/dumper/remove.hh"
   *  @brief Remove a file.
   *
   *  Remove a file transmitted by some provider.
   */
  class          remove : public io::data {
  public:
                 remove();
                 remove(remove const& right);
                 ~remove();
    remove&      operator=(remove const& right);
    unsigned int type() const;
    static unsigned int
                 static_type();

    unsigned int instance_id;
    QString      tag;
    QString      filename;

    static mapping::entry const
                    entries[];
    static io::event_info::event_operations const
                    operations;

  private:
    void         _internal_copy(remove const& right);
  };
}

CCB_END()

#endif // !CCB_DUMPER_REMOVE_HH
