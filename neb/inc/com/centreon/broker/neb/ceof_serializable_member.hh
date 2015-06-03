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
                     void(T::*unserialize)(std::string const&));
                   ~ceof_serializable_member();

    virtual void   serialize(T const& object, ceof_writer& writer) const;
    virtual void   unserialize(T& object, ceof_iterator& iterator) const;

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
