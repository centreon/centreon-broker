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
  char const* _name;
  char const* _name_v2;
  source* _ptr;
  bool _serialize;
  std::shared_ptr<source> _source;
  source::source_type _type;

 public:
  enum attribute {
    always_valid = 0,
    invalid_on_zero = (1 << 0),
    invalid_on_minus_one = (1 << 1),
    invalid_on_v2 = (1 << 2),
    invalid_on_v3 = (1 << 3)
  };

  /**
   *  @brief Constructor.
   *
   *  Build an entry from a property.
   *
   *  @param[in] name Entry name.
   *  @param[in] prop Property.
   */
  template <typename T, typename U>
  entry(U(T::*prop),
        char const* name,
        uint32_t attr = always_valid,
        bool serialize = true,
        char const* name_v2 = nullptr)
      : _attribute(attr),
        _name(name),
        _name_v2(name_v2),
        _serialize(serialize) {
    if (!_name_v2 && !(attr & invalid_on_v2))
      _name_v2 = _name;
    _source = std::make_shared<property<T>>(prop, &_type);
    _ptr = _source.get();
  }

  /**
   *  Default constructor.
   */
  entry()
      : _attribute(always_valid),
        _name(nullptr),
        _name_v2(nullptr),
        _ptr(nullptr),
        _serialize(false),
        _type(source::UNKNOWN) {}

  /**
   *  Copy constructor.
   *
   *  @param[in] other  Object to copy.
   */
  entry(entry const& other)
      : _attribute(other._attribute),
        _name(other._name),
        _name_v2(other._name_v2),
        _ptr(other._ptr),
        _serialize(other._serialize),
        _source(other._source),
        _type(other._type) {}
  ~entry() = default;
  entry& operator=(entry const&) = delete;
  constexpr uint32_t get_attribute() { return _attribute; }
  bool get_bool(io::data const& d) const;
  double get_double(io::data const& d) const;
  int get_int(io::data const& d) const;
  /**
   *  Get the name of this entry.
   *
   *  @return The name of this entry.
   */
  constexpr char const* get_name() { return _name; }
  /**
   *  Get the name of this entry in version 2.x.
   *
   *  @return The name of this entry in version 2.x.
   */
  constexpr char const* get_name_v2() { return _name_v2; }
/**
 *  Check if entry is to be serialized.
 *
 *  @return True if entry is to be serialized.
 */
  constexpr bool get_serialize() { return _serialize; }
  short get_short(io::data const& d) const;
  std::string const& get_string(io::data const& d) const;
  timestamp const& get_time(io::data const& d) const;
/**
 *  Get entry type.
 *
 *  @return Entry type.
 */
  constexpr uint32_t get_type() { return _type; }
  uint32_t get_uint(io::data const& d) const;
  unsigned short get_ushort(io::data const& d) const;
/**
 *  Get if this entry is a null entry.
 *
 *  @return  True if this entry is a null entry (last entry).
 */
  constexpr bool is_null() { return _type == source::UNKNOWN; }
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
