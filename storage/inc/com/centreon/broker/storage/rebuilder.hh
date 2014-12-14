/*
** Copyright 2012-2014 Merethis
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

#  include <ctime>
#  include <memory>
#  include <QThread>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
class               database;

namespace           storage {
  /**
   *  @class rebuilder rebuilder.hh "com/centreon/broker/storage/rebuilder.hh"
   *  @brief Check for graphs to be rebuild.
   *
   *  Check for graphs to be rebuild at fixed interval.
   */
  class             rebuilder : public QThread {
  public:
                    rebuilder(
                      database_config const& db_cfg,
                      unsigned int rebuild_check_interval = 600,
                      time_t interval_length = 60,
                      unsigned int rrd_length = 15552000);
                    ~rebuilder() throw ();
    void            exit() throw ();
    unsigned int    get_interval() const throw ();
    time_t          get_interval_length() const throw ();
    unsigned int    get_rrd_length() const throw ();
    void            run();

  private:
                    rebuilder(rebuilder const& other);
    rebuilder&      operator=(rebuilder const& other);
    void            _rebuild_metric(
                      database& db,
                      unsigned int metric_id,
                      QString const& metric_name,
                      short metric_type,
                      unsigned int interval,
                      unsigned length);
    void            _rebuild_status(
                       database& db,
                       unsigned int index_id,
                       unsigned int interval);
    void            _send_rebuild_event(
                      bool end,
                      unsigned int id,
                      bool is_index);
    void            _set_index_rebuild(
                      database& db,
                      unsigned int index_id,
                      short state);

    database_config _db_cfg;
    unsigned int    _interval;
    time_t          _interval_length;
    unsigned int    _rrd_len;
    volatile bool   _should_exit;
  };
}

CCB_END()

#endif // !CCB_STORAGE_REBUILDER_HH
