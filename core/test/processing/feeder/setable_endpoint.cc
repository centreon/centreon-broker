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
  _initial_count = se._initial_count;
  _initial_replay_events = se._initial_replay_events;
  _initial_store_events = se._initial_store_events;
  _opened_streams = se._opened_streams;
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
    _initial_count(0),
    _initial_replay_events(false),
    _initial_store_events(false),
    _opened_streams(0),
    _should_succeed(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] se Object to copy.
 */
setable_endpoint::setable_endpoint(setable_endpoint const& se)
  : io::endpoint(se) {
  _internal_copy(se);
}

/**
 *  Destructor.
 */
setable_endpoint::~setable_endpoint() {
  this->close();
}

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
  //set_process(false, false);
  return ;
}

/**
 *  Open endpoint.
 *
 *  @return New setable_stream.
 */
QSharedPointer<io::stream> setable_endpoint::open() {
  // Increment open attempts.
  ++_opened_streams;

  // Check if open should succeed.
  if (!_should_succeed)
    throw (exceptions::msg() << "setable endpoint should not succeed");

  // Open stream.
  QSharedPointer<setable_stream> ss(
    new setable_stream);
  ss->set_count(_initial_count);
  ss->set_replay_events(_initial_replay_events);
  ss->set_store_events(_initial_store_events);
  _streams.push_back(ss);
  return (ss.staticCast<io::stream>());
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
 *  Set initial stream count.
 *
 *  @param[in] cnt Initial count.
 */
void setable_endpoint::set_initial_count(unsigned int cnt) {
  _initial_count = cnt;
  return ;
}

/**
 *  Set initial replay events feature.
 *
 *  @param[in] replay true to make streams replay events.
 */
void setable_endpoint::set_initial_replay_events(bool replay) {
  _initial_replay_events = replay;
  return ;
}

/**
 *  Set initial store events feature.
 *
 *  @param[in] store true to make streams store events.
 */
void setable_endpoint::set_initial_store_events(bool store) {
  _initial_store_events = store;
  return ;
}

/**
 *  Set process.
 *
 *  @param[in] in  Set to true to enable streams to process input
 *                 events.
 *  @param[in] out Set to true to enable streams to process output
 *                 events.
 */
void setable_endpoint::set_process(bool in, bool out) {
  for (QList<QSharedPointer<setable_stream> >::iterator
         it = _streams.begin(),
         end = _streams.end();
       it != end;
       ++it)
    (*it)->process(in, out);
  return ;
}

/**
 *  Set whether or not the endpoint should fail.
 *
 *  @param[in] should_succeed Set to true if endpoint should succeed,
 *                            false otherwise.
 */
void setable_endpoint::set_succeed(bool succeed) {
  _should_succeed = succeed;
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
