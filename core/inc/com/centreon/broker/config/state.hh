/*
** Copyright 2011-2012,2017 Centreon
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

#  include <list>
#  include <map>
#  include <string>
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
                                  state(state const& other);
                                  ~state();
    state&                        operator=(state const& other);
    void                          broker_id(int id) throw ();
    int                           broker_id() const throw ();
    void                          broker_name(std::string const& name);
    std::string const&            broker_name() const throw ();
    void                          cache_directory(std::string const& dir);
    std::string const&            cache_directory() const throw ();
    void                          command_file(std::string const& file);
    std::string const&            command_file() const throw();
    void                          command_protocol(std::string const& prot);
    std::string const&            command_protocol() const throw();
    void                          clear();
    std::list<endpoint>&          endpoints() throw ();
    std::list<endpoint> const&    endpoints() const throw ();
    void                          event_queue_max_size(
                                    int val) throw ();
    int                           event_queue_max_size() const throw ();
    void                          flush_logs(bool flush) throw ();
    bool                          flush_logs() const throw ();
    void                          log_thread_id(bool log_id) throw ();
    bool                          log_thread_id() const throw ();
    void                          log_timestamp(logging::timestamp_type log_time) throw ();
    logging::timestamp_type       log_timestamp() const throw ();
    void                          log_human_readable_timestamp(
                                    bool human_log_time) throw ();
    bool                          log_human_readable_timestamp() const throw();
    std::list<logger>&            loggers() throw ();
    std::list<logger> const&      loggers() const throw ();
    std::string const&            module_directory() const throw ();
    void                          module_directory(
                                    std::string const& dir);
    std::list<std::string>&       module_list() throw ();
    std::list<std::string> const& module_list() const throw ();
    std::map<std::string, std::string>&
                                  params() throw ();
    std::map<std::string, std::string> const&
                                  params() const throw ();
    void                          poller_id(int id) throw ();
    int                           poller_id() const throw ();
    void                          poller_name(std::string const& name);
    std::string const&            poller_name() const throw ();

  private:
    void                          _internal_copy(state const& other);

    int                           _broker_id;
    std::string                   _broker_name;
    std::string                   _cache_directory;
    std::string                   _command_file;
    std::string                   _command_protocol;
    std::list<endpoint>           _endpoints;
    int                           _event_queue_max_size;
    bool                          _flush_logs;
    bool                          _log_thread_id;
    logging::timestamp_type       _log_timestamp;
    bool                          _log_human_readable_timestamp;
    std::list<logger>             _loggers;
    std::string                   _module_dir;
    std::list<std::string>        _module_list;
    std::map<std::string, std::string>
                                  _params;
    int                           _poller_id;
    std::string                   _poller_name;
  };
}

CCB_END()

#endif // !CCB_CONFIG_STATE_HH
