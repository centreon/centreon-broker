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
#ifndef CCB_MISC_FIFO_HH
#define CCB_MISC_FIFO_HH
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {

template <typename T, std::size_t N>
class mfifo {
  std::mutex _fifo_m;
  std::condition_variable _fifo_cv;

  /* When T objects arrive in the mfifo, two things are done.
   * A boolean is inserted at the end of the timeline with the value false.
   * A tuple is made with the T object, the index of the input source and a
   * pointer to the boolean.
   * This tuple is inserted at the end of the queue.
   * The idea behind all of this is we can treat events by order. When an event
   * is done, we have access to the boolean stored in the timeline to set it to
   * true.
   * And later, the input source that sent events will know how many events can
   * be released from the retention queue. */
  std::deque<std::tuple<T, uint32_t, bool*>> _events;

  /* Since we have N input sources, we must manage N queues.
   * So each one will know when its events will be released. */
  std::array<std::deque<bool>, N> _timeline;

  /* This array stores how many events for each connector have been
   * acknowledged. */
  std::array<int32_t, N> _ack;

  /* Number of elements not acknowledged. More precisely, elements are added
   * regularly to the fifo. When an element is pop from it, its corresponding
   * boolean is set to true. Those booleans are stored in the timeline array.
   * _pending_elements sums the number of elements in the timelines from the
   * first false element. */
  int32_t _pending_elements;

 public:
  mfifo() : _ack{0}, _pending_elements(0) {}

  std::deque<std::tuple<T, uint32_t, bool*>> const& get_events() const {
    return _events;
  }

  /**
   * @brief Returns a pointer to the first tuple contained in the fifo. If
   * not element is available, it returns nullptr.
   *
   * @return A pointer to the tuple or nullptr if none found.
   */
  std::deque<std::tuple<T, uint32_t, bool*>> first_events() {
    std::deque<std::tuple<T, uint32_t, bool*>> retval;
    std::lock_guard<std::mutex> lk(_fifo_m);
    std::swap(_events, retval);
    return retval;
  }

  /**
   * @brief Push a new element on this fifo coming from idx input source and
   * returns the number of elements already acknowledged.
   *
   * @param idx The input source
   * @param e The element to add
   *
   * @return The number of elements to ack.
   */
  int32_t push(uint32_t idx, T e) {
    std::lock_guard<std::mutex> lk(_fifo_m);
    _pending_elements++;
    _timeline[idx].push_back(false);
    _events.emplace_back(std::make_tuple(e, idx, &_timeline[idx].back()));
    int32_t retval = _ack[idx];
    _ack[idx] = 0;
    return retval;
  }

  int32_t get_acks(uint32_t idx) {
    std::lock_guard<std::mutex> lk(_fifo_m);
    int32_t retval = _ack[idx];
    _ack[idx] = 0;
    return retval;
  }

  /**
   * @brief Count for an input source how many consecutive elements have been.
   * treated, remove them and returns that count.
   *
   * @param idx The input source index.
   */
  void clean(uint32_t idx) {
    std::lock_guard<std::mutex> lk(_fifo_m);
    uint32_t count = 0;
    auto& t = _timeline[idx];
    while (!t.empty() && t.front()) {
      t.pop_front();
      ++count;
    }
    _pending_elements -= count;
    _ack[idx] += count;
  }

  /**
   * @brief Return the timeline of the idx input source.
   *
   * @param idx The input source.
   *
   * @return a reference to the timeline.
   */
  std::deque<bool> const& get_timeline(uint32_t idx) const {
    return _timeline[idx];
  }

  int32_t get_pending_elements() const { return _pending_elements; }
};
}  // namespace misc

CCB_END()

#endif /* !CCB_MISC_FIFO_HH */
