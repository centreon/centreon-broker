/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_CEOF_CEOF_VISITOR_HH
#define CCB_CEOF_CEOF_VISITOR_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_visitor ceof_visitor.hh
 * "com/centreon/broker/ceof/ceof_visitor.hh"
 *  @brief Centreon Engine Object File visitor.
 *
 *  Too clever by a half?
 */
class ceof_visitor {
 public:
  ceof_visitor(bool must_serialize, bool must_unserialize)
      : _must_serialize(must_serialize), _must_unserialize(must_unserialize) {}

  virtual ~ceof_visitor() throw() {}

  template <typename T>
  void visit(T& object,
             std::string const& name,
             std::string (T::*serialize_func)() const,
             void (T::*deserialize_fun)(std::string const&)) {
    (void)name;
    if (_must_serialize)
      serialize_helper(object, name, serialize_func);
    if (_must_unserialize)
      deserialize_helper(object, name, deserialize_fun);
  }

  template <typename T>
  void serialize_helper(T& object,
                        std::string const& name,
                        std::string (T::*f)() const) {
    std::string val = ((object).*f)();
    serialize(name, val);
  }

  template <typename T>
  void deserialize_helper(T& object,
                          std::string const& name,
                          void (T::*f)(std::string const&)) {
    std::string val = deserialize(name);
    ((object).*f)(val);
  }

 protected:
  virtual void serialize(std::string const& name, std::string const& value) {
    (void)name;
    (void)value;
  }
  virtual std::string deserialize(std::string const& name) {
    (void)name;
    return ("");
  }

 private:
  ceof_visitor(ceof_visitor const&);
  ceof_visitor& operator=(ceof_visitor const&);

  bool _must_serialize;
  bool _must_unserialize;
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_WRITER_HH
