/*
 * Copyright 2021 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#if !defined _CCB_MISC_BUFFER_HH
#define _CCB_MISC_BUFFER_HH

#include <deque>
#include <vector>
#include <stdexcept>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
/**
 * @brief This is an attempt to build a fast buffer of characters. Internally,
 * it is a deque of std::vector<char>. When we append a vector to it, this one
 * is just append to it as is. The other action we can do is to pop n elements
 * from it. This operation is more complicated since we create a vector which
 * is a vector of containing the first n elements of the buffer, then we play
 * with a _shift number that indicates the index of the first element in the
 * first vector, and in case we remove more elements that the size of the
 * first vectors, we remove those vectors one by one.
 */
class buffer {
  /**
   * @brief The index of the first element in the first vector, this to avoid
   * to reallocate this first vector each time we pop a character.
   */
  size_t _shift;
  /**
   * @brief The number of characters in the buffer.
   */
  size_t _size;
  std::deque<std::vector<char>> _buf;

 public:
  /**
   * @brief Default constructor.
   */
  buffer() : _shift{0}, _size{0} {}

  /**
   * @brief append a vector to the buffer. This function is very fast since
   * the vector is just kept as is and added to the queue.
   *
   * @param a
   */
  void push(std::vector<char> a) {
    if (!a.empty()) {
      _size += a.size();
      _buf.emplace_back(std::move(a));
    }
  }

  std::pair<char*, size_t> front() {
    return std::make_pair(_buf.front().data() + _shift, _buf.front().size() - _shift);
  }

  std::vector<char> pop() {
    if (_buf.empty())
      return std::vector<char>();

    if (_shift == 0) {
      auto retval = std::move(_buf.front());
      _buf.pop_front();
      _size -= retval.size();
      return retval;
    }
    else {
      std::vector<char> retval(_buf.front().begin() + _shift, _buf.front().end());
      _size -= retval.size();
      _shift = 0;
      _buf.pop_front();
      return retval;
    }
  }

  /**
   * @brief Pop the n first characters from the buffer as a vector<char>. If n
   * is greater than the buffer size, all will be converted as a single
   * vector.
   *
   * @param n The number of characters to pop/extract.
   *
   * @return a std::vector<char>
   */
  std::vector<char> pop(size_t n) {
    std::vector<char> retval;
    if (_buf.empty())
      return retval;

    auto it = _buf.begin();
    if (it->size() - _shift <= n) {
      if (_shift == 0) {
        _size -= it->size();
        retval = std::move(*it);
      } else {
        retval = std::vector<char>(it->begin() + _shift, it->end());
        _size -= it->size() - _shift;
        _shift = 0;
      }
      _buf.pop_front();
      it = _buf.begin();
    } else {
      retval =
          std::vector<char>(it->begin() + _shift, it->begin() + _shift + n);
      _shift += n;
      _size -= n;
      return retval;
    }
    while (retval.size() < n && it != _buf.end()) {
      if (it->size() + retval.size() <= n) {
        _size -= it->size();
        retval.insert(retval.end(), std::make_move_iterator(it->begin()),
                      std::make_move_iterator(it->end()));
        _buf.pop_front();
        it = _buf.begin();
      } else {
        size_t length = n - retval.size();
        retval.insert(retval.end(), it->begin(), it->begin() + length);
        _shift += length;
        _size -= length;
      }
    }
    return retval;
  }

  /**
   * @brief Return the total size of the buf in bytes.
   *
   * @return the size of the buf.
   */
  size_t size() const { return _size; }

  /**
   * @brief Return if the buffer is empty.
   *
   * @return True if it is empty, false otherwise.
   */
  bool empty() const { return _buf.empty(); }

  /**
   * @brief This function is interesting to debug. We can get any character
   * in the buffer just from its index. We should not abuse of this function
   * since it is not a very fast function.
   *
   * @param idx An integer.
   *
   * @return The character at the idx position in the buffer.
   */
  char& operator[](int idx) {
    if (idx < 0)
      throw std::out_of_range("indexation cannot be negative");

    if (_buf.empty())
      throw std::out_of_range("buffer empty");

    auto it = _buf.begin();
    if (static_cast<uint32_t>(idx) < it->size() - _shift)
      return (*it)[_shift + idx];

    idx -= it->size() - _shift;
    ++it;
    while (static_cast<uint32_t>(idx) >= it->size() && it != _buf.end()) {
      idx -= it->size();
      ++it;
    }

    if (it == _buf.end())
      throw std::out_of_range("index too large");
    return (*it)[idx];
  }
};
}  // namespace misc

CCB_END()

#endif /* !_CCB_MISC_BUFFER_HH */
