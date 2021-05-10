/*
** Copyright 2021 Centreon
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

#ifndef CCB_IO_EXTENSION_HH
#define CCB_IO_EXTENSION_HH

#include <string>
#include <unordered_map>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace io {

/**
 * @class extension extension.hh "com/centreon/broker/bbdo/extension.hh"
 * @brief Used essentially by the bbdo stream, it stores some configuration
 * about sub streams of a stream.
 *
 */
class extension {
    std::string _name;
    bool _optional;
    bool _mandatory;
    std::unordered_map<std::string, std::string> _options;

  public:
    extension() : _optional{false}, _mandatory{false} {}

    extension(const std::string& name, bool optional, bool mandatory) :
      _name{name},
      _optional{optional},
      _mandatory{mandatory} {}

    extension(const extension& other) : _name{other._name}, _optional{other._optional}, _mandatory{other._mandatory}, _options{other._options} {}

    extension(extension&& other) : _name{std::move(other._name)}, _optional{other._optional}, _mandatory{other._mandatory}, _options{std::move(other._options)} {}

    extension& operator=(extension&& ext) {
      if (&ext != this) {
        _name = std::move(ext._name);
        _optional = ext._optional;
        _mandatory = ext._mandatory;
        _options = std::move(ext._options);
      }
      return *this;
    }

    extension& operator=(const extension& ext) {
      if (&ext != this) {
        _name = ext._name;
        _optional = ext._optional;
        _mandatory = ext._mandatory;
        _options = ext._options;
      }
      return *this;
    }

    std::unordered_map<std::string, std::string>& mutable_options() {
      return _options;
    }
    const std::unordered_map<std::string, std::string>& options() const {
      return _options;
    }
    const std::string& name() const { return _name; }
    bool is_optional() const { return _optional; }
    bool is_mandatory() const { return _mandatory; }
};
}

CCB_END()
#endif /* !CCB_IO_EXTENSION_HH */
