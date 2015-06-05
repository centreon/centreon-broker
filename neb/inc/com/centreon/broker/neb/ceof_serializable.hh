/*
** Copyright 2009-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
                   ceof_serializable() {
                     init_bindings();
                   }

                   ceof_serializable(ceof_serializable const&) {
                     init_bindings();
                   }
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

    virtual void    init_bindings() {}

  private:
    std::map<std::string, misc::shared_ptr<ceof_serializable_member<T> > >
                    _members;
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_SERIALIZABLE_HH
