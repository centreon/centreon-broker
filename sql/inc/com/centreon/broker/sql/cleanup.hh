/*
** Copyright 2014 Merethis
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

#ifndef CCB_SQL_CLEANUP_HH
#  define CCB_SQL_CLEANUP_HH

#  include <QSqlDatabase>
#  include <QThread>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         sql {
  /**
   *  @class cleanup cleanup.hh "com/centreon/broker/sql/cleanup.hh"
   *  @brief Check to cleanup database.
   *
   *  Check to cleanup database.
   */
  class           cleanup : public QThread {
  public:
                  cleanup();
                  cleanup(cleanup const& right);
                  ~cleanup() throw ();
    cleanup&      operator=(cleanup const& right);
    void          exit() throw ();
    unsigned int  get_interval() const throw ();
    void          run();
    void          set_db(QSqlDatabase const& db);
    void          set_interval(unsigned int interval) throw ();

  private:
    void          _internal_copy(cleanup const& right);

    QSqlDatabase  _db;
    unsigned int  _interval;
    volatile bool _should_exit;
  };
}

CCB_END()

#endif // !CCB_SQL_CLEANUP_HH
