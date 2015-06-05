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
#include "com/centreon/broker/neb/ceof_serializable.hh"

using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
template <typename T>
ceof_serializable<T>::ceof_serializable() {
  if (_members.empty())
    init_bindings();
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
template <typename T>
ceof_serializable<T>::ceof_serializable(ceof_serializable const& other) {
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           Reference to this object.
 */
template <typename T>
ceof_serializable<T>& ceof_serializable<T>::operator=(
                        ceof_serializable const& other) {
  if (this != &other) {
  }
  return (*this);
}

/**
 *  Destructor.
 */
template <typename T>
ceof_serializable<T>::~ceof_serializable() {

}

template <typename T>
/**
 *  Add a member.
 *
 *  @param[in] name         The name of the member.
 *  @param[in] serialize    The method used to serialize the member.
 *  @param[in] unserialize  The method used to unserialize the member.
 */
void ceof_serializable<T>::add_member(
  std::string const& name,
  std::string (T::*serialize)() const,
  void(T::*unserialize)(std::string const&)) {
  _members[name] = misc::make_shared(
    new ceof_serializable_member<T>(serialize, unserialize));
}

/**
 *  Serialize the serializable.
 *
 *  @param[in] object   The object to serialize.
 *  @param[out] writer  The writer.
 */
template <typename T>
void ceof_serializable<T>::serialize(T const& object, ceof_writer& writer) {
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

/**
 *  Unserialize the serializable.
 *
 *  @param[out] object   The object to unserialize
 *  @param[in] iterator  The token iterator.
 */
template <typename T>
void ceof_serializable<T>::unserialize(T& object, ceof_iterator iterator) {
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
