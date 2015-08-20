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

#ifndef CCB_DUMPER_DB_DUMP_COMMITTED_HH
#  define CCB_DUMPER_DB_DUMP_COMMITTED_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               dumper {
  /**
   *  @class db_dump_committed db_dump_committed.hh "com/centreon/broker/dumper/db_dump_committed.hh"
   *  @brief DB dump committed event.
   *
   *  A general notification that the database just has been dumped,
   *  and endpoints should maybe reload their configurations.
   */
  class                 db_dump_committed : public io::data {
  public:
                        db_dump_committed();
                        db_dump_committed(db_dump_committed const& other);
                        ~db_dump_committed();
    db_dump_committed&  operator=(db_dump_committed const& other);
    unsigned int        type() const;

  private:
    void                _internal_copy(db_dump_committed const& other);
  };
}

CCB_END()

#endif // !CCB_DUMPER_DB_DUMP_COMMITTED_HH
