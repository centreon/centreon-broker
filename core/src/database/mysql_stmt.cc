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

#include "com/centreon/broker/database/mysql_stmt.hh"
#include <cfloat>
#include <cmath>
#include <functional>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

mysql_stmt::mysql_stmt() : _id(0), _param_count(0) {}

mysql_stmt::mysql_stmt(std::string const& query, bool named) {
  mysql_bind_mapping bind_mapping;
  std::hash<std::string> hash_fn;
  if (named) {
    std::string q;
    q.reserve(query.size());
    bool in_string(false);
    char open(0);
    int size(0);
    for (std::string::const_iterator it(query.begin()), end(query.end());
         it != end; ++it) {
      if (in_string) {
        if (*it == '\\') {
          q.push_back(*it);
          it++;
          q.push_back(*it);
        } else {
          q.push_back(*it);
          if (*it == open)
            in_string = false;
        }
      } else {
        if (*it == ':') {
          std::string::const_iterator itt(it + 1);
          while (itt != end && (isalnum(*itt) || *itt == '_'))
            ++itt;
          std::string key(it, itt);
          mysql_bind_mapping::iterator fkit(bind_mapping.find(key));
          if (fkit != bind_mapping.end()) {
            int value(fkit->second);
            bind_mapping.erase(fkit);
            key.push_back('1');
            bind_mapping.insert(std::make_pair(key, value));
            key[key.size() - 1] = '2';
            bind_mapping.insert(std::make_pair(key, size));
          } else
            bind_mapping.insert(std::make_pair(std::string(it, itt), size));

          ++size;
          it = itt - 1;
          q.push_back('?');
        } else {
          if (*it == '\'' || *it == '"') {
            in_string = true;
            open = *it;
          }
          q.push_back(*it);
        }
      }
    }
    _id = hash_fn(q);
    _query = q;
    _bind_mapping = bind_mapping;
    _param_count = bind_mapping.size();
  } else {
    _id = hash_fn(query);
    _query = query;

    // How many '?' in the query, we don't count '?' in strings.
    _param_count = _compute_param_count(query);
  }
}

mysql_stmt::mysql_stmt(std::string const& query,
                       mysql_bind_mapping const& bind_mapping)
    : _id(std::hash<std::string>{}(query)),
      _query(query),
      _bind_mapping(bind_mapping) {
  if (bind_mapping.empty())
    _param_count = _compute_param_count(query);
  else
    _param_count = bind_mapping.size();
}

/**
 *  Move constructor
 */
mysql_stmt::mysql_stmt(mysql_stmt&& other)
    : _id(other._id),
      _param_count(other._param_count),
      _query(other._query),
      _bind(std::move(other._bind)),
      _bind_mapping(other._bind_mapping) {}

mysql_stmt& mysql_stmt::operator=(mysql_stmt const& other) {
  if (this != &other) {
    _id = other._id;
    _param_count = other._param_count;
    _query = other._query;
    _bind_mapping = other._bind_mapping;
  }
  return *this;
}

int mysql_stmt::_compute_param_count(std::string const& query) {
  int retval(0);
  bool in_string(false), jocker(false);
  for (std::string::const_iterator it(query.begin()), end(query.end());
       it != end; ++it) {
    if (!in_string) {
      if (*it == '?')
        ++retval;
      else if (*it == '\'' || *it == '"')
        in_string = true;
    } else {
      if (jocker)
        jocker = false;
      else if (*it == '\\')
        jocker = true;
      else if (*it == '\'' || *it == '"')
        in_string = false;
    }
  }
  return retval;
}

bool mysql_stmt::prepared() const {
  return _id != 0;
}

int mysql_stmt::get_id() const {
  return _id;
}

std::unique_ptr<database::mysql_bind> mysql_stmt::get_bind() {
  return std::move(_bind);
}

void mysql_stmt::operator<<(io::data const& d) {
  // Get event info.
  io::event_info const* info(io::events::instance().get_event_info(d.type()));
  if (info) {
    // FIXME DBR...
    bool db_v2(true);  //_db.schema_version() == database::v2);
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null(); ++current_entry) {
      char const* entry_name;
      if (db_v2)
        entry_name = current_entry->get_name_v2();
      else
        entry_name = current_entry->get_name();
      if (entry_name && entry_name[0]) {
        std::string field(":");
        field.append(entry_name);
        switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            bind_value_as_bool(field, current_entry->get_bool(d));
            break;
          case mapping::source::DOUBLE:
            bind_value_as_f64(field, current_entry->get_double(d));
            break;
          case mapping::source::INT: {
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
          } break;
          case mapping::source::SHORT:
            bind_value_as_i32(field, current_entry->get_short(d));
            break;
          case mapping::source::STRING: {
            std::string v(current_entry->get_string(d));
            if (current_entry->get_attribute() ==
                mapping::entry::invalid_on_zero) {
              if (v == "")
                bind_value_as_null(field);
              else
                bind_value_as_str(field, v);
            } else
              bind_value_as_str(field, v);
          } break;
          case mapping::source::TIME: {
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
          } break;
          case mapping::source::UINT: {
            unsigned int v(current_entry->get_uint(d));
            switch (current_entry->get_attribute()) {
              case mapping::entry::invalid_on_zero:
                //              if (v == 0)
                //                bind_value_as_null(field);
                //              else
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
          } break;
          default:  // Error in one of the mappings.
            throw(exceptions::msg() << "invalid mapping for object "
                                    << "of type '" << info->get_name()
                                    << "': " << current_entry->get_type()
                                    << " is not a known type ID");
        };
      }
    }
  } else
    throw(exceptions::msg() << "cannot bind object of type " << d.type()
                            << " to database query: mapping does not exist");
}

void mysql_stmt::bind_value_as_i32(int range, int value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_i32(range, value);
}

void mysql_stmt::bind_value_as_i32(std::string const& name, int value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_i32(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_i32(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_i32(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

void mysql_stmt::bind_value_as_u32(int range, unsigned int value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_u32(range, value);
}

void mysql_stmt::bind_value_as_u32(std::string const& name,
                                   unsigned int value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_u32(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_u32(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_u32(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

void mysql_stmt::bind_value_as_u64(int range, unsigned long long value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_u64(range, value);
}

void mysql_stmt::bind_value_as_u64(std::string const& name,
                                   unsigned long long value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_u64(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_u64(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_u64(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

/**
 *  Bind the value to the variable at index range.
 *
 * @param range The index in the statement.
 * @param value The value to bind. It can be Inf or NaN.
 */
void mysql_stmt::bind_value_as_f32(int range, float value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_f32(range, value);
}

void mysql_stmt::bind_value_as_f32(std::string const& name, float value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_f32(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_f32(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_f32(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

/**
 *  Bind the value to the variable at index range.
 *
 * @param range The index in the statement.
 * @param value The value to bind. It can be Inf or NaN.
 */
void mysql_stmt::bind_value_as_f64(int range, double value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_f64(range, value);
}

void mysql_stmt::bind_value_as_f64(std::string const& name, double value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_f64(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_f64(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_f64(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

void mysql_stmt::bind_value_as_tiny(int range, char value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_tiny(range, value);
}

void mysql_stmt::bind_value_as_tiny(std::string const& name, char value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_tiny(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_tiny(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_tiny(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

void mysql_stmt::bind_value_as_bool(int range, bool value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_bool(range, value);
}

void mysql_stmt::bind_value_as_bool(std::string const& name, bool value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_bool(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_bool(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_bool(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

void mysql_stmt::bind_value_as_str(int range, std::string const& value) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_str(range, value);
}

void mysql_stmt::bind_value_as_str(std::string const& name,
                                   std::string const& value) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_str(it->second, value);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_str(it->second, value);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_str(it->second, value);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

void mysql_stmt::bind_value_as_null(int range) {
  if (!_bind)
    _bind.reset(new database::mysql_bind(_param_count));
  _bind->set_value_as_null(range);
}

void mysql_stmt::bind_value_as_null(std::string const& name) {
  mysql_bind_mapping::iterator it(_bind_mapping.find(name));
  if (it != _bind_mapping.end()) {
    bind_value_as_null(it->second);
  } else {
    std::string key(name);
    key.append("1");
    it = _bind_mapping.find(key);
    if (it != _bind_mapping.end()) {
      bind_value_as_null(it->second);
      key[key.size() - 1] = '2';
      it = _bind_mapping.find(key);
      if (it != _bind_mapping.end()) {
        bind_value_as_null(it->second);
        return;
      }
    }
    logging::debug(logging::low) << "mysql: cannot bind object with name '"
                                 << name << "' in statement " << get_id();
  }
}

std::string const& mysql_stmt::get_query() const {
  return _query;
}

int mysql_stmt::get_param_count() const {
  return _param_count;
}
