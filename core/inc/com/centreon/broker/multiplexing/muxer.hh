/*
** Copyright 2009-2017 Centreon
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

#ifndef CCB_MULTIPLEXING_MUXER_HH
#define CCB_MULTIPLEXING_MUXER_HH

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_set>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_file.hh"

CCB_BEGIN()

namespace multiplexing {
/**
 *  @class muxer muxer.hh "com/centreon/broker/multiplexing/muxer.hh"
 *  @brief Receive and send events from/to the multiplexing engine.
 *
 *  This class is a cornerstone in event multiplexing. Each endpoint
 *  willing to communicate with the multiplexing engine will create a
 *  new muxer object. This objects broadcast events sent to it to all
 *  other muxer objects.
 *
 *  @see engine
 */
class muxer : public io::stream {
 public:
  typedef std::unordered_set<uint32_t> filters;

  muxer(std::string const& name, bool persistent = false);
  muxer(muxer const& other) = delete;
  muxer& operator=(muxer const& other) = delete;
  ~muxer();
  void ack_events(int count);
  static void event_queue_max_size(uint32_t max) noexcept;
  static uint32_t event_queue_max_size() throw();
  void publish(std::shared_ptr<io::data> const& d);
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void set_read_filters(filters const& fltrs);
  void set_write_filters(filters const& fltrs);
  filters const& get_read_filters() const;
  filters const& get_write_filters() const;
  const std::string& get_read_filters_str() const;
  const std::string& get_write_filters_str() const;
  uint32_t get_event_queue_size() const;
  void nack_events();
  void remove_queue_files();
  void statistics(json11::Json::object& tree) const;
  void wake();
  int write(std::shared_ptr<io::data> const& d);

  static std::string memory_file(std::string const& name);
  static std::string queue_file(std::string const& name);

 private:
  void _clean();
  void _get_event_from_file(std::shared_ptr<io::data>& event);
  std::string _memory_file() const;
  void _push_to_queue(std::shared_ptr<io::data> const& event);
  std::string _queue_file() const;

  std::condition_variable _cv;
  std::list<std::shared_ptr<io::data>> _events;
  uint32_t _events_size;
  static uint32_t _event_queue_max_size;
  std::unique_ptr<persistent_file> _file;
  mutable std::mutex _mutex;
  std::string _name;
  bool _persistent;
  std::list<std::shared_ptr<io::data>>::iterator _pos;
  filters _read_filters;
  filters _write_filters;
  std::string _read_filters_str;
  std::string _write_filters_str;
};
}  // namespace multiplexing

CCB_END()

#endif  // !CCB_MULTIPLEXING_MUXER_HH
