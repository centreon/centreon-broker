/*
** Copyright 2011-2012,2015 Centreon
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

#ifndef CCB_MAPPING_ENTRY_HH
#define CCB_MAPPING_ENTRY_HH

#include <cassert>
#include <memory>

#include "com/centreon/broker/mapping/property.hh"

CCB_BEGIN()

namespace mapping {
/**
 *  @class entry entry.hh "com/centreon/broker/mapping/entry.hh"
 *  @brief Mapping class.
 *
 *  Holds a member with a name and a data source.
 */
class entry {
  const uint32_t _attribute;
  char const* _name_v2;
  const bool _serialize;
  source* _source;
  const source::source_type _type;

 public:
  enum attribute {
    always_valid = 0,
    invalid_on_zero = (1 << 0),
    invalid_on_minus_one = (1 << 1)
  };

  /**
   * @brief String constructor.
   *
   * @tparam T Property container class.
   * @param T::*prop The property to point to.
   * @param name The name of the property.
   * @param max_len Its max size or 0 if unlimited.
   * @param attr A bit field about the property validity.
   * @param serialize Is this property sent to the database.
   */
  template <typename T>
  entry(std::string(T::*prop),
        char const* name,
        size_t max_len,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new sproperty<T>(prop, max_len)),
        _type(source::STRING) {}

  /**
   *  @brief Boolean constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T>
  entry(bool(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::BOOL) {}

  /**
   *  @brief Double constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T>
  entry(double(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::DOUBLE) {}

  /**
   *  @brief Unsigned integer constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T>
  entry(uint32_t(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::UINT) {}

  /**
   *  @brief Integer constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T>
  entry(int(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::INT) {}

  /**
   *  @brief Unsigned short constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T>
  entry(unsigned short(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::USHORT) {}

  /**
   *  @brief Short constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T>
  entry(short(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::SHORT) {}

  /**
   *  @brief Time constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   */
  template <typename T>
  entry(timestamp(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true)
      : _attribute(attr),
        _name_v2(name),
        _serialize(serialize),
        _source(new property<T>(prop)),
        _type(source::TIME) {}

  /**
   *  Default constructor.
   */
  entry()
      : _attribute(always_valid),
        _name_v2(nullptr),
        _serialize(false),
        _source(nullptr),
        _type(source::UNKNOWN) {}

  entry(const entry&) = delete;
  entry(entry&& other)
      : _attribute(other._attribute),
        _name_v2(other._name_v2),
        _serialize(other._serialize),
        _source(other._source),
        _type(other._type) {
    other._source = nullptr;
  }

  ~entry() noexcept {
    if (_source) {
      delete _source;
      _source = nullptr;
    }
  }
  entry& operator=(entry const&) = delete;
  uint32_t get_attribute() const { return _attribute; }
  bool get_bool(const io::data& d) const;
  double get_double(const io::data& d) const;
  int get_int(const io::data& d) const;

  /**
   *  Get the name of this entry in version 2.x.
   *
   *  @return The name of this entry in version 2.x.
   */
  const char* get_name_v2() const { return _name_v2; }
  /**
   *  Check if entry is to be serialized.
   *
   *  @return True if entry is to be serialized.
   */
  bool get_serialize() const { return _serialize; }
  short get_short(const io::data& d) const;
  std::string const& get_string(const io::data& d,
                                size_t* max_len = nullptr) const;
  timestamp const& get_time(const io::data& d) const;
  /**
   *  Get entry type.
   *
   *  @return Entry type.
   */
  uint32_t get_type() const { return _type; }
  uint32_t get_uint(const io::data& d) const;
  unsigned short get_ushort(const io::data& d) const;
  /**
   *  Get if this entry is a null entry.
   *
   *  @return  True if this entry is a null entry (last entry).
   */
  bool is_null() const { return _type == source::UNKNOWN; }
  void set_bool(io::data& d, bool value) const;
  void set_double(io::data& d, double value) const;
  void set_int(io::data& d, int value) const;
  void set_short(io::data& d, short value) const;
  void set_string(io::data& d, std::string const& value) const;
  void set_time(io::data& d, timestamp const& value) const;
  void set_uint(io::data& d, uint32_t value) const;
  void set_ushort(io::data& d, unsigned short value) const;
};
}  // namespace mapping

CCB_END()

#endif  // !CCB_MAPPING_ENTRY_HH
