/*
** Copyright 2011-2012 Centreon
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
    void                          command_file(QString const& path);
    QString const&                command_file() const throw ();
    void                          event_queue_max_size(
                                    unsigned int val) throw ();
    unsigned int                  event_queue_max_size() const throw ();
    void                          flush_logs(bool flush) throw ();
    bool                          flush_logs() const throw ();
    QList<endpoint>&              inputs() throw ();
    QList<endpoint> const&        inputs() const throw ();
    void                          instance_id(unsigned int id) throw ();
    unsigned int                  instance_id() const throw ();
    void                          instance_name(QString const& name) throw ();
    QString const&                instance_name() const throw ();
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
    endpoint&                     temporary() throw ();
    endpoint const&               temporary() const throw ();

  private:
    void                          _internal_copy(state const& s);

    QString                       _command_file;
    unsigned int                  _event_queue_max_size;
    bool                          _flush_logs;
    QList<endpoint>               _inputs;
    unsigned int                  _instance_id;
    QString                       _instance_name;
    bool                          _log_thread_id;
    bool                          _log_timestamp;
    QList<logger>                 _loggers;
    QString                       _module_dir;
    QList<QString>                _module_list;
    QList<endpoint>               _outputs;
    QMap<QString, QString>        _params;
    endpoint                      _temporary;
  };
}

CCB_END()

#endif // !CCB_CONFIG_STATE_HH
