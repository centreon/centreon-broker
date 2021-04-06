/*
** Copyright 2020 Centreon
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

#ifndef CCB_STATS_CENTER_HH
#define CCB_STATS_CENTER_HH

#include <asio.hpp>
#include <functional>
#include <unordered_map>

#include "broker.pb.h"
#include "com/centreon/broker/pool.hh"

CCB_BEGIN()

namespace stats {
/**
 * @brief Centralize Broker statistics.
 *
 * It works with the Broker thread pool. To avoid mutexes, we use a strand.
 * Then each modification in stats is serialized through the strand. The big
 * interest is that statistics are always written asynchronously and the
 * impact on the software is almost nothing.
 *
 * The idea is that even there are several threads used to fill Broker
 * statistics, never two updates are done at the same time. The strand forces
 * an update to be done after another one. And then we don't need mutex to
 * protect data.
 *
 * To help the software developer to use this strand not directly available
 * to the developer, member functions are declared.
 *
 * Examples:
 * * update(&EndpointStats::set_queued_events, _stats, value)
 *   calls throw the thread pool the EndpointStats::set_queued_events member
 *   function of the EndpointStats* _stats object with the value value.
 * * update(_stats->mutable_state(), state)
 *   sets the std::string state() of the _stats EndpointStats object to the
 *   value value.
 */
class center {
  static center* _instance;
  asio::io_context::strand _strand;
  BrokerStats _stats;
  int _json_stats_file_creation;

  center();
  ~center();

 public:
  static center& instance();
  static void load();
  static void unload();
  std::string to_string();

  // EndpointStats* register_endpoint(const std::string& name);
  EngineStats* register_engine();
  // FeederStats* register_feeder(EndpointStats* ep_stats,
  //                             const std::string& name);
  // ConflictManagerStats* register_conflict_manager();
  // MysqlManagerStats* register_mysql_manager();
  // MysqlConnectionStats* register_mysql_connection(MysqlManagerStats* s);
  ModuleStats* register_modules(void);
  // bool unregister_endpoint(const std::string& name);
  // bool unregister_feeder(EndpointStats* ep_stats, const std::string& name);
  // bool unregister_mysql_connection(MysqlConnectionStats* c);
  // bool unregister_mysql_manager(void);
  int get_json_stats_file_creation(void);

  void get_stats(const StatsQuery* request, BrokerStats* response);

  /**
   * @brief Set the value pointed by ptr to the value value.
   *
   * @tparam T The template class.
   * @param ptr A pointer to object of type T
   * @param value The value of type T to set.
   */
  template <typename T>
  void update(T* ptr, T value) {
    _strand.post([ptr, value = std::move(value)] { *ptr = value; });
  }

  /**
   * @brief Almost the same function as in the previous case, but with a
   * Timestamp object. And we can directly set a time_t value.
   *
   * @param ptr A pointer to object of type Timestamp.
   * @param value The value of type time_t to set.
   */
  // void update(google::protobuf::Timestamp* ptr, time_t value) {
  //  _strand.post([ptr, value] {
  //    ptr->Clear();
  //    ptr->set_seconds(value);
  //  });
  //}

  /**
   * @brief Sometimes with protobuf, we can not access to a mutable pointer.
   * For example with int32 values. We have instead a setter member function.
   * To be able to call it, we provide this method that needs the method
   * to call, a pointer to the EndpointStats object and the value to set.
   *
   * @tparam T The type of the value to set.
   * @param U::*f A member function of U.
   * @param ptr A pointer to an existing U.
   * @param value The value to set.
   */
  template <typename U, typename T>
  void update(void (U::*f)(T), U* ptr, T value) {
    _strand.post([ptr, f, value] { (ptr->*f)(value); });
  }

  void execute(std::function<void()> f) { _strand.post(f); }

  template <typename U, typename T>
  const T& get(T (U::*f)() const, const U* ptr) {
    std::promise<T> p;
    std::future<T> retval = p.get_future();
    _strand.post([&p, ptr, f] { p.set_value((ptr->*f)()); });
    return retval.get();
  }
};

}  // namespace stats

CCB_END()

#endif /* !CCB_STATS_CENTER_HH */
