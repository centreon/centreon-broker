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

#ifndef CCB_NEB_CEOF_SERIALIZABLE_MEMBER_HH
#  define CCB_NEB_CEOF_SERIALIZABLE_MEMBER_HH

#  include <string>
#  include <map>
#  include "com/centreon/broker/neb/ceof_iterator.hh"
#  include "com/centreon/broker/neb/ceof_writer.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   neb {
  /**
   *  @class ceof_serializable_member ceof_serializable_member.hh "com/centreon/broker/neb/ceof_serializable_member.hh"
   *  @brief Represent a serializable Centreon Engine Object File member.
   */
  template <typename T>
  class            ceof_serializable_member {
  public:
                   ceof_serializable_member(
                     std::string (T::*serialize)() const,
                     void(T::*unserialize)(std::string const&))
                     : _serialize(serialize),
                       _unserialize(unserialize) {}
                   ~ceof_serializable_member() {}

    void serialize(T const& object, ceof_writer& writer) const {
      writer.add_value((object.*_serialize)());
    }
    void unserialize(T& object, ceof_iterator& iterator) const {
      (object.*_unserialize)(iterator.get_value());
    }

  private:
    std::string    (T::*_serialize)() const;
    void           (T::*_unserialize)(std::string const&);

                   ceof_serializable_member();
                   ceof_serializable_member(
                     ceof_serializable_member const&);
    ceof_serializable_member&
                   operator=(ceof_serializable_member const&);
  };
}

CCB_END()

#endif // !CCB_NEB_CEOF_SERIALIZABLE_MEMBER_HH
