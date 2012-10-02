/*
** Copyright 2012 Merethis
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

#ifndef CCB_STORAGE_REBUILDER_HH
#  define CCB_STORAGE_REBUILDER_HH

#  include <memory>
#  include <QSqlDatabase>
#  include <QThread>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         storage {
  /**
   *  @class rebuilder rebuilder.hh "com/centreon/broker/storage/rebuilder.hh"
   *  @brief Check for graphs to be rebuild.
   *
   *  Check for graphs to be rebuild at fixed interval.
   */
  class           rebuilder : public QThread {
  public:
                  rebuilder();
                  rebuilder(rebuilder const& right);
                  ~rebuilder() throw ();
    rebuilder&    operator=(rebuilder const& right);
    void          exit() throw ();
    unsigned int  get_interval() const throw ();
    void          run();
    void          set_db(QSqlDatabase const& db);
    void          set_interval(unsigned int interval) throw ();

  private:
    void          _internal_copy(rebuilder const& right);

    QSqlDatabase  _db;
    unsigned int  _interval;
    volatile bool _should_exit;
  };
}

CCB_END()

#endif // !CCB_STORAGE_REBUILDER_HH
