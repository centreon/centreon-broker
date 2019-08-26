/*
 * Copyright 2019 Centreon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 */

#ifndef CCB_MISC_VARIANT_HH_
#define CCB_MISC_VARIANT_HH_

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace misc {
/**
 *  @class variant variant.hh "com/centreon/misc/variant.hh"
 *  @brief Provide a type safe union.
 *
 *  Variant is a simple way to store any kind of value among string, int or
 * double.
 */
class variant {
 public:
  enum meta_type {
    type_none,
    type_bool,
    type_int,
    type_uint,
    type_long,
    type_ulong,
    type_double,
    type_string,
  };
  variant();
  variant(bool value);
  variant(int32_t value);
  variant(uint32_t value);
  variant(int64_t value);
  variant(uint64_t value);
  variant(double value);
  variant(char const* value);
  variant(std::string const& value);
  variant(variant const& variant);
  ~variant();
  variant& operator=(variant const& other);

  meta_type user_type() const;
  bool as_bool() const;
  int32_t as_int() const;
  uint32_t as_uint() const;
  int64_t as_long() const;
  uint64_t as_ulong() const;
  double as_double() const;
  std::string const& as_string() const;

 private:
  meta_type _type;
  union {
    bool _bool_value;
    int32_t _int_value;
    uint32_t _uint_value;
    int64_t _long_value;
    uint64_t _ulong_value;
    double _dbl_value;
    std::string _str_value;
  };
};
}  // namespace misc

CCB_END()

#endif /* !CCB_MISC_VARIANT_HH */
