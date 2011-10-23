/*
** Copyright 2011 Merethis
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
#include "setable_endpoint.hh"
#include "setable_stream.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] se Object to copy.
 */
void setable_endpoint::_internal_copy(setable_endpoint const& se) {
  _opened_streams = se._opened_streams;
  _save_streams = se._save_streams;
  _should_succeed = se._should_succeed;
  _streams = se._streams;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
setable_endpoint::setable_endpoint()
  : io::endpoint(false),
    _opened_streams(0),
    _save_streams(false),
    _should_succeed(new volatile bool) {}

/**
 *  Copy constructor.
 *
 *  @param[in] se Object to copy.
 */
setable_endpoint::setable_endpoint(setable_endpoint const& se)
  : com::centreon::broker::io::endpoint(se) {
  _internal_copy(se);
}

/**
 *  Destructor.
 */
setable_endpoint::~setable_endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] se Object to copy.
 *
 *  @return This object.
 */
setable_endpoint& setable_endpoint::operator=(setable_endpoint const& se) {
  if (&se != this) {
    com::centreon::broker::io::endpoint::operator=(se);
    _internal_copy(se);
  }
  return (*this);
}

/**
 *  Close endpoint.
 */
void setable_endpoint::close() {
  *_should_succeed = false;
  return ;
}

/**
 *  Open endpoint.
 *
 *  @return New setable_stream.
 */
QSharedPointer<com::centreon::broker::io::stream> setable_endpoint::open() {
  ++_opened_streams;
  if (!*_should_succeed)
    throw (exceptions::msg() << "setable endpoint should not succeed");
  QSharedPointer<com::centreon::broker::io::stream> s;
  if (_save_streams) {
    QSharedPointer<setable_stream> ss(
      new setable_stream(_should_succeed));
    _streams.push_back(ss);
    s = ss.staticCast<com::centreon::broker::io::stream>();
  }
  else
    s = QSharedPointer<com::centreon::broker::io::stream>(
          new setable_stream(_should_succeed));
  return (s);
}

/**
 *  Get the number of open attempts.
 *
 *  @return Number of times open() was called.
 */
unsigned int setable_endpoint::opened_streams() const {
  return (_opened_streams);
}

/**
 *  Enable or disable stream save.
 *
 *  @param[in] save Set to true to save streams.
 */
void setable_endpoint::save_streams(bool save) {
  _save_streams = save;
  return ;
}

/**
 *  Set whether or not the endpoint should fail.
 *
 *  @param[in] should_succeed Set to true if endpoint should succeed,
 *                            false otherwise.
 */
void setable_endpoint::set(bool should_succeed) {
  *_should_succeed = should_succeed;
  return ;
}

/**
 *  Get opened streams.
 *
 *  @return Opened streams.
 */
QList<QSharedPointer<setable_stream> >& setable_endpoint::streams() {
  return (_streams);
}
