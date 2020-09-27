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

#ifndef CCB_MAPPING_SOURCE_HH
#define CCB_MAPPING_SOURCE_HH

#include <string>

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace mapping {
/**
 *  @class source source.hh "com/centreon/broker/mapping/source.hh"
 *  @brief Internal mapping class.
 *
 *  This class is used internally by the mapping engine and
 *  should not be used otherwise.
 */
class source {
 public:
  enum source_type {
    UNKNOWN = 0,
    BOOL,
    DOUBLE,
    INT,
    SHORT,
    STRING,
    TIME,
    UINT,
    USHORT,
    ULONG
  };

  /**
   *  Default constructor.
   */
  source() {}

  /**
   *  Destructor.
   */
  virtual ~source() noexcept {}
  source(source const&) = delete;
  source& operator=(source const&) = delete;
  virtual bool get_bool(io::data const& d) = 0;
  virtual double get_double(io::data const& d) = 0;
  virtual int get_int(io::data const& d) = 0;
  virtual short get_short(io::data const& d) = 0;
  virtual std::string const& get_string(io::data const& d, size_t* max_len) = 0;
  virtual timestamp const& get_time(io::data const& d) = 0;
  virtual uint32_t get_uint(io::data const& d) = 0;
  virtual unsigned short get_ushort(io::data const& d) = 0;
  virtual void set_bool(io::data& d, bool value) = 0;
  virtual void set_double(io::data& d, double value) = 0;
  virtual void set_int(io::data& d, int value) = 0;
  virtual void set_short(io::data& d, short value) = 0;
  virtual void set_string(io::data& d, std::string const& value) = 0;
  virtual void set_time(io::data& d, timestamp const& value) = 0;
  virtual void set_uint(io::data& d, uint32_t value) = 0;
  virtual void set_ushort(io::data& d, unsigned short value) = 0;
};
}  // namespace mapping

CCB_END()

#endif  // !CCB_MAPPING_SOURCE_HH
