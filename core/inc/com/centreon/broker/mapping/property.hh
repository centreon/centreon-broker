/*
** Copyright 2011 Centreon
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
 private:
  union {
    bool T::*b;
    double T::*d;
    int T::*i;
    short T::*s;
    std::string T::*q;
    timestamp T::*t;
    unsigned int T::*I;
    unsigned short T::*S;
    uint64_t T::*l;
  } _prop;

 public:
  /**
   *  Boolean constructor.
   *
   *  @param[in]  b Boolean property.
   *  @param[out] t If not NULL, set to BOOL.
   */
  property(bool(T::*b), source_type* t) {
    _prop.b = b;
    if (t)
      *t = BOOL;
  }

  /**
   *  Double constructor.
   *
   *  @param[in]  d Double property.
   *  @param[out] t If not NULL, set to DOUBLE.
   */
  property(double(T::*d), source_type* t) {
    _prop.d = d;
    if (t)
      *t = DOUBLE;
  }

  /**
   *  integer constructor.
   *
   *  @param[in]  i integer property.
   *  @param[out] t if not null, set to int.
   */
  property(int(T::*i), source_type* t) {
    _prop.i = i;
    if (t)
      *t = INT;
  }

  /**
   *  Short constructor.
   *
   *  @param[in]  s Short property.
   *  @param[out] t If not NULL, set to SHORT.
   */
  property(short(T::*s), source_type* t) {
    _prop.s = s;
    if (t)
      *t = SHORT;
  }

  /**
   *  String constructor.
   *
   *  @param[in]  q String property.
   *  @param[out] t If not NULL, set to STRING.
   */
  property(std::string(T::*q), source_type* t) {
    _prop.q = q;
    if (t)
      *t = STRING;
  }

  /**
   *  Time constructor.
   *
   *  @param[in]  tt Time property.
   *  @param[out] t  If not NULL, set to TIME.
   */
  property(timestamp(T::*ts), source_type* t) {
    _prop.t = ts;
    if (t)
      *t = TIME;
  }

  /**
   *  Unsigned integer constructor.
   *
   *  @param[in]  I Unsigned integer property.
   *  @param[out] t If not NULL, set to UINT.
   */
  property(unsigned int(T::*I), source_type* t) {
    _prop.I = I;
    if (t)
      *t = UINT;
  }

  /**
   *  Unsigned short constructor.
   *
   *  @param[in]  S Unsigned short property.
   *  @param[out] t If not NULL, set to USHORT.
   */
  property(unsigned short(T::*S), source_type* t) {
    _prop.S = S;
    if (t)
      *t = USHORT;
  }

  /**
   *  Copy constructor.
   *
   *  @param[in] other Object to copy.
   */
  property(property const& other) : source(other), _prop(other._prop) {}

  /**
   *  Destructor.
   */
  ~property() {}

  /**
   *  Assignment operator.
   *
   *  @param[in] other Object to copy.
   *
   *  @return This object.
   */
  property& operator=(property const& other) {
    _prop = other._prop;
    return (*this);
  }

  /**
   *  Get a boolean property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Boolean property.
   */
  bool get_bool(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.b));
  }

  /**
   *  Get a double property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Double property.
   */
  double get_double(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.d));
  }

  /**
   *  Get an integer property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Integer property.
   */
  int get_int(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.i));
  }

  /**
   *  Get a short property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Short property.
   */
  short get_short(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.s));
  }

  /**
   *  Get a string property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return String property.
   */
  std::string const& get_string(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.q));
  }

  /**
   *  Get a time property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Time property.
   */
  timestamp const& get_time(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.t));
  }

  /**
   *  Get an unsigned integer property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Unsigned integer property.
   */
  unsigned int get_uint(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.I));
  }

  /**
   *  Get an unsigned short property.
   *
   *  @param[in] d Object to get from.
   *
   *  @return Unsigned short property.
   */
  unsigned short get_ushort(io::data const& d) {
    return (static_cast<T const*>(&d)->*(_prop.S));
  }

  /**
   *  Set a boolean property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_bool(io::data& d, bool value) {
    static_cast<T*>(&d)->*(_prop.b) = value;
    return;
  }

  /**
   *  Set a double property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_double(io::data& d, double value) {
    static_cast<T*>(&d)->*(_prop.d) = value;
    return;
  }

  /**
   *  Set an integer property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_int(io::data& d, int value) {
    static_cast<T*>(&d)->*(_prop.i) = value;
    return;
  }

  /**
   *  Set a short property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_short(io::data& d, short value) {
    static_cast<T*>(&d)->*(_prop.s) = value;
    return;
  }

  /**
   *  Set a string property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_string(io::data& d, std::string const& value) {
    static_cast<T*>(&d)->*(_prop.q) = value;
    return;
  }

  /**
   *  Set a time property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_time(io::data& d, timestamp const& value) {
    static_cast<T*>(&d)->*(_prop.t) = value;
    return;
  }

  /**
   *  Set an unsigned integer property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_uint(io::data& d, unsigned int value) {
    static_cast<T*>(&d)->*(_prop.I) = value;
    return;
  }

  /**
   *  Set an unsigned short property.
   *
   *  @param[out] d     Object to set.
   *  @param[in]  value New value.
   */
  void set_ushort(io::data& d, unsigned short value) {
    static_cast<T*>(&d)->*(_prop.S) = value;
    return;
  }
};
}  // namespace mapping

CCB_END()

#endif  // !CCB_MAPPING_PROPERTY_HH
