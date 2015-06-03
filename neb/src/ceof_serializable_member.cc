/*
** Copyright 2009-2013,2015 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/ceof_serializable_member.hh"

using namespace com::centreon::broker::neb;

template <typename T>
/**
 *  Constructor.
 *
 *  @param[in] serialize    The serialize member.
 *  @param[in] unserialize  The unserialize member.
 */
ceof_serializable_member<T>::ceof_serializable_member(
                               std::string const& (T::*serialize)(),
                               void(T::*unserialize)(std::string const&))
  : _serialize(serialize),
    _unserialize(unserialize) {
}

/**
 *  Destructor.
 */
template <typename T>
ceof_serializable_member<T>::~ceof_serializable_member() {

}

/**
 *  Serialize the member.
 */
template <typename T>
void  ceof_serializable_member<T>::serialize(
                                    T const& object,
                                    ceof_writer& writer) const {
  writer.add_value((object.*_serialize)());
}

/**
 *  Unserialize the member.
 */
template <typename T>
void ceof_serializable_member<T>::unserialize(
                                    T& object,
                                    ceof_iterator& iterator) const {
  (object.*_unserialize)(iterator.get_value());
}
