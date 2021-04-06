/*
** Copyright 2011, 2021 Centreon
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

#ifndef CCB_MAPPING_PROPERTY_HH
#define CCB_MAPPING_PROPERTY_HH

#include "com/centreon/broker/mapping/source.hh"

CCB_BEGIN()

namespace mapping {
/**
 *  @class property property.hh "com/centreon/broker/mapping/property.hh"
 *  @brief Internal property-mapping class.
 *
 *  This class is used internally by the mapping engine and
 *  should not be used otherwise.
 */
template <typename T>
class property : public source {
 protected:
  union {
    bool T::*b;
    double T::*d;
    int T::*i;
    short T::*s;
    std::string T::*q;
    timestamp T::*t;
    uint32_t T::*I;
    unsigned short T::*S;
    uint64_t T::*l;
  } _prop;

 public:
  /**
   *  Boolean constructor.
   *
   *  @param[in]  b Boolean property.
   */
  property(bool(T::*b)) { _prop.b = b; }

  /**
   *  Double constructor.
   *
   *  @param[in]  d Double property.
   */
  property(double(T::*d)) { _prop.d = d; }

  /**
   *  integer constructor.
   *
   *  @param[in]  i integer property.
   */
  property(int32_t(T::*i)) { _prop.i = i; }

  /**
   *  Short constructor.
   *
   *  @param[in]  s Short property.
   */
  property(short(T::*s)) { _prop.s = s; }

  /**
   *  Time constructor.
   *
   *  @param[in]  tt Time property.
   */
  property(timestamp(T::*ts)) { _prop.t = ts; }

  /**
   *  String constructor.
   *
   *  @param[in]  q String property.
   */
  property(std::string(T::*q)) { _prop.q = q; }

  /**
   *  Unsigned integer constructor.
   *
   *  @param[in]  I Unsigned integer property.
   */
  property(uint32_t(T::*I)) { _prop.I = I; }

  /**
   *  Unsigned integer 64 bits constructor.
   *
   *  @param[in]  uint64 property.
   */
  property(uint64_t(T::*l)) { _prop.l = l; }

  /**
   *  Unsigned short constructor.
   *
   *  @param[in]  S Unsigned short property.
   */
  property(unsigned short(T::*S)) { _prop.S = S; }

  /**
   *  Destructor.
   */
  ~property() noexcept {}

  property& operator=(property const&) = delete;
  property(const property&) = delete;

  /**
   *  Get a boolean property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Boolean property.
   */
  bool get_bool(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.b);
  }

  /**
   *  Get a double property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Double property.
   */
  double get_double(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.d);
  }

  /**
   *  Get an integer property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Integer property.
   */
  int get_int(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.i);
  }

  /**
   *  Get a short property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Short property.
   */
  short get_short(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.s);
  }

  /**
   *  Get a string property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return String property.
   */
  std::string const& get_string(io::data const& d,
                                size_t* max_len __attribute__((unused))) {
    return static_cast<T const*>(&d)->*(_prop.q);
  }

  /**
   *  Get a time property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Time property.
   */
  timestamp const& get_time(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.t);
  }

  /**
   *  Get an uint32_teger property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Unsigned integer property.
   */
  uint32_t get_uint(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.I);
  }

  /**
   *  Get an uint64_teger property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Unsigned integer property.
   */
  uint64_t get_ulong(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.l);
  }

  /**
   *  Get an unsigned short property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Unsigned short property.
   */
  unsigned short get_ushort(io::data const& d) {
    return static_cast<T const*>(&d)->*(_prop.S);
  }

  /**
   *  Set a boolean property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_bool(io::data& d, bool value) {
    static_cast<T*>(&d)->*(_prop.b) = value;
  }

  /**
   *  Set a double property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_double(io::data& d, double value) {
    static_cast<T*>(&d)->*(_prop.d) = value;
  }

  /**
   *  Set an integer property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_int(io::data& d, int value) {
    static_cast<T*>(&d)->*(_prop.i) = value;
  }

  /**
   *  Set a short property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_short(io::data& d, short value) {
    static_cast<T*>(&d)->*(_prop.s) = value;
  }

  /**
   *  Set a string property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_string(io::data& d, std::string const& value) {
    static_cast<T*>(&d)->*(_prop.q) = value;
  }

  /**
   *  Set a time property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_time(io::data& d, timestamp const& value) {
    static_cast<T*>(&d)->*(_prop.t) = value;
  }

  /**
   *  Set an uint32_teger property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_uint(io::data& d, uint32_t value) {
    static_cast<T*>(&d)->*(_prop.I) = value;
  }

  /**
   *  Set an uint64_teger property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_ulong(io::data& d, uint64_t value) {
    static_cast<T*>(&d)->*(_prop.l) = value;
  }

  /**
   *  Set an unsigned short property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_ushort(io::data& d, unsigned short value) {
    static_cast<T*>(&d)->*(_prop.S) = value;
  }
};

template <typename T>
class sproperty : public property<T> {
  const size_t _max_len;

 public:
  /**
   *  String constructor.
   *
   *  @param[in]  q String property.
   *  @param[in]  max_len This value is used for serialization. If the string
   *  is longer, it will be truncated to be stored in the database unless the
   *  value is 0.
   */
  sproperty(std::string(T::*q), size_t max_len)
      : property<T>(q), _max_len(max_len) {}

  /**
   *  Get a string property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return String property.
   */
  std::string const& get_string(io::data const& d, size_t* max_len) {
    if (max_len)
      *max_len = _max_len;
    return property<T>::get_string(d, max_len);
  }
};

}  // namespace mapping

CCB_END()

#endif  // !CCB_MAPPING_PROPERTY_HH
