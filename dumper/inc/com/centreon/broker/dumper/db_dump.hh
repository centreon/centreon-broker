/*
** Copyright 2015 Merethis
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

#ifndef CCB_DUMPER_DB_DUMP_HH
#  define CCB_DUMPER_DB_DUMP_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class db_dump db_dump.hh "com/centreon/broker/dumper/db_dump.hh"
   *  @brief DB dump event.
   *
   *  Notify that a DB dump is starting or stopping.
   */
  class                 db_dump : public io::data {
  public:
                        db_dump();
                        db_dump(db_dump const& other);
                        ~db_dump();
    db_dump&            operator=(db_dump const& other);
    unsigned int        type() const;

    bool                commit;
    bool                full;
    unsigned int        poller_id;

  private:
    void                _internal_copy(db_dump const& other);
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_DUMP_HH
