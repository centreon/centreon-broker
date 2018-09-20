/*
** Copyright 2018 Centreon
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

#include <cfloat>
#include <cstring>
#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mysql_bind.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

my_bool mysql_bind::_true = 1;

mysql_bind::mysql_bind(int size) {
  set_size(size);
}

mysql_bind::mysql_bind(io::data const& d) {
  // Get event info.
  io::event_info const*
    info(io::events::instance().get_event_info(d.type()));
  if (info) {
    //FIXME DBR...
    bool db_v2(true);   //_db.schema_version() == database::v2);
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null();
         ++current_entry) {
      char const* entry_name;
      if (db_v2)
        entry_name = current_entry->get_name_v2();
      else
        entry_name = current_entry->get_name();
      if (entry_name
          && entry_name[0]) {
          //          FIXME DBR
//          && (_excluded.find(entry_name) == _excluded.end())) {
        std::string field(":");
        field.append(entry_name);
        switch (current_entry->get_type()) {
        case mapping::source::BOOL:
          set_value_as_bool(field, current_entry->get_bool(d));
          break;
        case mapping::source::DOUBLE:
          set_value_as_f64(field, current_entry->get_double(d));
          break;
        case mapping::source::INT:
          {
            int v(current_entry->get_int(d));
            switch (current_entry->get_attribute()) {
            case mapping::entry::invalid_on_zero:
              if (v == 0)
                set_value_as_null(field);
              else
                set_value_as_i32(field, v);
              break;
            case mapping::entry::invalid_on_minus_one:
              if (v == -1)
                set_value_as_null(field);
              else
                set_value_as_i32(field, v);
              break;
            default:
              set_value_as_i32(field, v);
            }
          }
          break;
        case mapping::source::SHORT:
          set_value_as_i32(field, current_entry->get_short(d));
          break;
        case mapping::source::STRING:
          {
            std::string v(current_entry->get_string(d).toStdString());
            if (current_entry->get_attribute()
                == mapping::entry::invalid_on_zero) {
              if (v == "")
                set_value_as_null(field);
              else
                set_value_as_str(field, v);
            }
            else
              set_value_as_str(field, v);
          }
          break;
        case mapping::source::TIME:
          {
            time_t v(current_entry->get_time(d));
            switch (current_entry->get_attribute()) {
            case mapping::entry::invalid_on_zero:
              if (v == 0)
                set_value_as_null(field);
              else
                set_value_as_u32(field, v);
              break;
            case mapping::entry::invalid_on_minus_one:
              if (v == -1)
                set_value_as_null(field);
              else
                set_value_as_u32(field, v);
              break;
            default:
              set_value_as_u32(field, v);
            }
          }
          break ;
        case mapping::source::UINT:
          {
            unsigned int v(current_entry->get_uint(d));
            switch (current_entry->get_attribute()) {
            case mapping::entry::invalid_on_zero:
              if (v == 0)
                set_value_as_null(field);
              else
                set_value_as_u32(field, v);
              break;
            case mapping::entry::invalid_on_minus_one:
              if (v == (unsigned int)-1)
                set_value_as_null(field);
              else
                set_value_as_u32(field, v);
              break;
            default:
              set_value_as_u32(field, v);
            }
          }
          break ;
        default: // Error in one of the mappings.
          throw (exceptions::msg() << "invalid mapping for object "
                 << "of type '" << info->get_name() << "': "
                 << current_entry->get_type()
                 << " is not a known type ID");
        };
      }
    }
  }
  else
    throw (exceptions::msg() << "cannot bind object of type "
           << d.type() << " to database query: mapping does not exist");
}

mysql_bind::mysql_bind(mysql_bind const& other)
 : _buffer(other._buffer),
   _length(other._length) {
  int size(_length.size());
  _bind.resize(size);
  for (int i(0); i < size; ++i) {
    memset(&_bind[i], 0, sizeof(MYSQL_BIND));
    _bind[i].buffer_type = other._bind[i].buffer_type;
    _bind[i].buffer = const_cast<char*>(_buffer[i].c_str());
    if (_length[i])
      _bind[i].length = &_length[i];
  }
}

mysql_bind::~mysql_bind() {}

void mysql_bind::set_size(int size) {
  _bind.resize(size);
  _buffer.resize(size);
  _length.resize(size);
}

void mysql_bind::set_value_as_str(std::string const& name, std::string const& value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_str(range, value);
}

void mysql_bind::set_value_as_str(int range, std::string const& value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_STRING;
  _buffer[range] = value;
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _length[range] = value.size();
  _bind[range].length = &_length[range];
}

void mysql_bind::set_value_as_tiny(std::string const& name, char value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_tiny(range, value);
}

void mysql_bind::set_value_as_tiny(int range, char value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_TINY;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_value_as_bool(std::string const& name, bool value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_bool(range, value);
}

void mysql_bind::set_value_as_bool(int range, bool value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_TINY;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_value_as_i32(std::string const& name, int value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_i32(range, value);
}

void mysql_bind::set_value_as_i32(int range, int value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_value_as_u32(std::string const& name, unsigned int value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_u32(range, value);
}

void mysql_bind::set_value_as_u32(int range, unsigned int value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _bind[range].is_unsigned = true;
}

void mysql_bind::set_value_as_f32(std::string const& name, float value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_f32(range, value);
}

void mysql_bind::set_value_as_f32(int range, float value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  if (isnan(value) || isinf(value))
    _bind[range].buffer_type = MYSQL_TYPE_NULL;
  else {
    _bind[range].buffer_type = MYSQL_TYPE_FLOAT;
    char* tmp = reinterpret_cast<char*>(&value);
    _buffer[range].resize(sizeof(value));
    memcpy(&_buffer[range][0], tmp, sizeof(value));
    _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  }
}

void mysql_bind::set_value_as_f64(std::string const& name, double value) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_f64(range, value);
}

void mysql_bind::set_value_as_f64(int range, double value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  if (isnan(value) || isinf(value))
    _bind[range].buffer_type = MYSQL_TYPE_NULL;
  else {
    _bind[range].buffer_type = MYSQL_TYPE_DOUBLE;
    char* tmp = reinterpret_cast<char*>(&value);
    _buffer[range].resize(sizeof(value));
    memcpy(&_buffer[range][0], tmp, sizeof(value));
    _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  }
}

void mysql_bind::set_value_as_null(std::string const& name) {
  int range(_bind.size());
  _name_bind[name] = range;
  set_value_as_null(range);
}

void mysql_bind::set_value_as_null(int range) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_NULL;
}

MYSQL_BIND const* mysql_bind::get_bind() const {
  return &_bind[0];
}
