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

#ifndef CCB_NEB_CEOF_SERIALIZABLE_HH
#  define CCB_NEB_CEOF_SERIALIZABLE_HH

#  include <string>
#  include <map>
#  include "com/centreon/broker/neb/ceof_iterator.hh"
#  include "com/centreon/broker/neb/ceof_writer.hh"
#  include "com/centreon/broker/neb/ceof_serializable_member.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class ceof_serializable ceof_serializable.hh "com/centreon/broker/neb/ceof_serializable.hh"
   *  @brief Represent a serializable Centreon Engine Object File class.
   */
  template <typename T>
  class            ceof_serializable {
  public:
                   ceof_serializable() {}

                   ceof_serializable(ceof_serializable const& other)
                    : _members(other._members){}
    ceof_serializable&
                   operator=(ceof_serializable const&) {}
   virtual         ~ceof_serializable() {}

    void add_member(
           std::string const& name,
           std::string (T::*serialize)() const,
           void(T::*unserialize)(std::string const&)) {
      _members[name] = misc::make_shared(
        new ceof_serializable_member<T>(serialize, unserialize));
    }

    void serialize(T const& object, ceof_writer& writer) {
      for (typename std::map<std::string,
                    misc::shared_ptr<ceof_serializable_member<T> > >::const_iterator
             it = _members.begin(),
             end = _members.end();
           it != end;
           ++it) {
        writer.add_key(it->first);
        it->second->serialize(object, writer);
      }
    }

    void unserialize(T& object, ceof_iterator iterator) {
      for (; !iterator.end(); ++iterator) {
        std::string key = iterator.get_value();
        ++iterator;
        typename std::map<std::string,
                 misc::shared_ptr<ceof_serializable_member<T> > >::const_iterator
          found = _members.find(key);
        if (found == _members.end())
          throw (exceptions::msg()
                 << "couldn't find the member named '" << key << "'");
        found->second->unserialize(object, iterator);
      }
    }

    virtual void init_bindings() = 0;

  private:
    std::map<std::string, misc::shared_ptr<ceof_serializable_member<T> > >
                    _members;
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_SERIALIZABLE_HH
