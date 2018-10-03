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

#include <QHash>
#include <cfloat>
#include <cmath>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mysql_bind.hh"
#include "com/centreon/broker/mysql_stmt.hh"

using namespace com::centreon::broker;

mysql_stmt::mysql_stmt()
 : _id(0) {
  std::cout << "mysql_stmt: empty constructor..." << std::endl;
}

mysql_stmt::mysql_stmt(std::string const& query,
                       mysql_bind_mapping const& bind_mapping)
 : _id(qHash(QString(query.c_str()))),
   _query(query),
   _bind_mapping(bind_mapping) {}

mysql_stmt::mysql_stmt(mysql_stmt const& other)
 : _id(other._id),
   _query(other._query),
   _bind_mapping(other._bind_mapping) {
  std::cout << "mysql_stmt: constructor by copy..." << std::endl;
}

mysql_stmt::~mysql_stmt() {
  std::cout << "mysql_stmt: destructor" << std::endl;
}

mysql_stmt& mysql_stmt::operator=(mysql_stmt const& other) {
  std::cout << "mysql_stmt: operator=..." << std::endl;
  if (this != &other) {
    _id = other._id;
    _query = other._query;
    _bind_mapping = other._bind_mapping;
  }
  return *this;
}

bool mysql_stmt::prepared() const {
  return _id != 0;
}

int mysql_stmt::get_id() const {
  return _id;
}

std::auto_ptr<mysql_bind> mysql_stmt::get_bind() {
  return _bind;
}

void mysql_stmt::operator<<(io::data const& d) {
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
          bind_value_as_bool(field, current_entry->get_bool(d));
          break;
        case mapping::source::DOUBLE:
          bind_value_as_f64(field, current_entry->get_double(d));
          break;
        case mapping::source::INT:
          {
            int v(current_entry->get_int(d));
            switch (current_entry->get_attribute()) {
            case mapping::entry::invalid_on_zero:
              if (v == 0)
                bind_value_as_null(field);
              else
                bind_value_as_i32(field, v);
              break;
            case mapping::entry::invalid_on_minus_one:
              if (v == -1)
                bind_value_as_null(field);
              else
                bind_value_as_i32(field, v);
              break;
            default:
              bind_value_as_i32(field, v);
            }
          }
          break;
        case mapping::source::SHORT:
          bind_value_as_i32(field, current_entry->get_short(d));
          break;
        case mapping::source::STRING:
          {
            std::string v(current_entry->get_string(d).toStdString());
            if (current_entry->get_attribute()
                == mapping::entry::invalid_on_zero) {
              if (v == "")
                bind_value_as_null(field);
              else
                bind_value_as_str(field, v);
            }
            else
              bind_value_as_str(field, v);
          }
          break;
        case mapping::source::TIME:
          {
            time_t v(current_entry->get_time(d));
            switch (current_entry->get_attribute()) {
            case mapping::entry::invalid_on_zero:
              if (v == 0)
                bind_value_as_null(field);
              else
                bind_value_as_u32(field, v);
              break;
            case mapping::entry::invalid_on_minus_one:
              if (v == -1)
                bind_value_as_null(field);
              else
                bind_value_as_u32(field, v);
              break;
            default:
              bind_value_as_u32(field, v);
            }
          }
          break ;
        case mapping::source::UINT:
          {
            unsigned int v(current_entry->get_uint(d));
            switch (current_entry->get_attribute()) {
            case mapping::entry::invalid_on_zero:
              if (v == 0)
                bind_value_as_null(field);
              else
                bind_value_as_u32(field, v);
              break;
            case mapping::entry::invalid_on_minus_one:
              if (v == (unsigned int)-1)
                bind_value_as_null(field);
              else
                bind_value_as_u32(field, v);
              break;
            default:
              bind_value_as_u32(field, v);
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

void mysql_stmt::bind_value_as_i32(int range, int value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_i32(range, value);
}

void mysql_stmt::bind_value_as_i32(std::string const& name, int value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_i32(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_i32(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_i32(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_u32(int range, unsigned int value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_u32(range, value);
}

void mysql_stmt::bind_value_as_u32(std::string const& name, unsigned int value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_u32(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_u32(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_u32(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_u64(int range, unsigned long long value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_u64(range, value);
}

void mysql_stmt::bind_value_as_u64(std::string const& name, unsigned long long value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_u64(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_u64(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_u64(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_f32(int range, float value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_f32(range, value);
}

void mysql_stmt::bind_value_as_f32(std::string const& name, float value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_f32(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_f32(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_f32(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_f64(int range, double value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_f64(range, value);
}

void mysql_stmt::bind_value_as_f64(std::string const& name, double value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_f64(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_f64(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_f64(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_tiny(int range, char value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_tiny(range, value);
}

void mysql_stmt::bind_value_as_tiny(std::string const& name, char value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_tiny(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_tiny(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_tiny(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_bool(int range, bool value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_bool(range, value);
}

void mysql_stmt::bind_value_as_bool(std::string const& name, bool value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_bool(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_bool(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_bool(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_str(int range, std::string const& value) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_str(range, value);
}

void mysql_stmt::bind_value_as_str(std::string const& name, std::string const& value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = " << value
      << std::endl;

    bind_value_as_str(it->second, value);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_str(it->second, value);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_str(it->second, value);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}

void mysql_stmt::bind_value_as_null(int range) {
  if (!_bind.get())
    _bind.reset(new mysql_bind);
  _bind->set_value_as_null(range);
}

void mysql_stmt::bind_value_as_null(std::string const& name) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    std::cout << "Statement " << get_id() << " : "
      << "name = " << name << "; value = null"
      << std::endl;

    bind_value_as_null(it->second);
  }
  else {
    std::string key(name);
    key.append("1");
    it =_bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_null(it->second);
      key[key.size() - 1] = '2';
      it =_bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_null(it->second);
        return ;
      }
    }
    logging::debug(logging::low)
      << "mysql: cannot bind object with name '" << name << "' in statement "
      << get_id();
  }
}
