/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_CONFIG_STATE_HH
#  define CCB_CONFIG_STATE_HH

#  include <QList>
#  include <QMap>
#  include <QString>
#  include "com/centreon/broker/config/endpoint.hh"
#  include "com/centreon/broker/config/logger.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                         config {
  /**
   *  @class state state.hh "com/centreon/broker/config/state.hh"
   *  @brief Full configuration state.
   *
   *  A fully parsed configuration is represented within this class
   *  which holds mandatory parameters as well as optional parameters,
   *  along with object definitions.
   */
  class                           state {
  public:
                                  state();
                                  state(state const& s);
                                  ~state();
    state&                        operator=(state const& s);
    void                          clear();
    void                          flush_logs(bool flush) throw ();
    bool                          flush_logs() const throw ();
    QList<endpoint>&              inputs() throw ();
    QList<endpoint> const&        inputs() const throw ();
    void                          log_thread_id(bool log_id) throw ();
    bool                          log_thread_id() const throw ();
    void                          log_timestamp(bool log_time) throw ();
    bool                          log_timestamp() const throw ();
    QList<logger>&                loggers() throw ();
    QList<logger> const&          loggers() const throw ();
    QString const&                module_directory() const throw ();
    void                          module_directory(QString const& dir);
    QList<QString>&               module_list() throw ();
    QList<QString> const&         module_list() const throw ();
    QList<endpoint>&              outputs() throw ();
    QList<endpoint> const&        outputs() const throw ();
    QMap<QString, QString>&       params() throw ();
    QMap<QString, QString> const& params() const throw ();

  private:
    void                          _internal_copy(state const& s);

    bool                          _flush_logs;
    QList<endpoint>               _inputs;
    bool                          _log_thread_id;
    bool                          _log_timestamp;
    QList<logger>                 _loggers;
    QString                       _module_dir;
    QList<QString>                _module_list;
    QList<endpoint>               _outputs;
    QMap<QString, QString>        _params;
  };
}

CCB_END()

#endif // !CCB_CONFIG_STATE_HH
