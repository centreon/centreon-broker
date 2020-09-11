/*
** Copyright 2013,2017 Centreon
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

#ifndef CCB_BBDO_INPUT_HH
#define CCB_BBDO_INPUT_HH

//#include <memory>
//#include <mutex>
#include <deque>
#include <string>

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bbdo {
/**
 *  @class input input.hh "com/centreon/broker/bbdo/input.hh"
 *  @brief BBDO input source.
 *
 *  The class converts an input stream into events using the BBDO
 *  (Broker Binary Data Objects) protocol.
 */
class input : virtual public io::stream {
  class buffer {
    uint32_t _event_id;
    uint32_t _source_id;
    uint32_t _dest_id;

    /* All the read data are get in vectors. We chose to not cut those vectors
     * and just move them into the deque. */
    std::deque<std::vector<char>> _buf;

   public:
    buffer(uint32_t event_id,
           uint32_t source_id,
           uint32_t dest_id,
           std::vector<char>&& v)
        : _event_id(event_id), _source_id(source_id), _dest_id(dest_id) {
      _buf.push_back(v);
    }
    buffer(const buffer&) = delete;
    buffer(buffer&& other)
        : _event_id(other._event_id),
          _source_id(other._source_id),
          _dest_id(other._dest_id),
          _buf(std::move(other._buf)) {}

    buffer& operator=(const buffer&) = delete;
    buffer& operator=(buffer&& other) {
      if (this != &other) {
        _event_id = other._event_id;
        _source_id = other._source_id;
        _dest_id = other._dest_id;
        _buf = std::move(other._buf);
      }
      return *this;
    }
    ~buffer() noexcept = default;

    bool matches(uint32_t event_id, uint32_t source_id, uint32_t dest_id) const
        noexcept {
      return event_id == _event_id && source_id == _source_id &&
             dest_id == _dest_id;
    }

    std::vector<char> to_vector() {
      size_t s = 0;
      for (auto& v : _buf)
        s += v.size();
      std::vector<char> retval;
      retval.reserve(s);
      for (auto& v : _buf)
        retval.insert(retval.end(), v.begin(), v.end());
      _buf.clear();
      return retval;
    }

    void push_back(std::vector<char>&& v) { _buf.push_back(v); }
    uint32_t get_event_id() const { return _event_id; }
  };

  /* If during a packet reading, we get several ones, this vector is useful
   * to keep in cache all but the first one. It will be read before a call
   * to _read_packet(). */
  std::vector<char> _packet;

  /* We good get parts of BBDO packets in the wrong order, this deque is useful
   * to paste parts together in the good order. */
  std::deque<buffer> _buffer;
  int _skipped;

  // void _buffer_must_have_unprocessed(int bytes, time_t deadline =
  // (time_t)-1);
  void _read_packet(size_t size, time_t deadline = (time_t)-1);

 public:
  input();
  virtual ~input();
  input(input const& other) = delete;
  input& operator=(input const& other) = delete;
  virtual bool read(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  bool read_any(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  virtual void acknowledge_events(uint32_t events) = 0;
};
}  // namespace bbdo

CCB_END()

#endif  // !CCB_BBDO_INPUT_HH
