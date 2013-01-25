/*
** Copyright 2013 Merethis
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

#include <memory>
#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/input.hh"
#include "com/centreon/broker/bbdo/output.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/processing/feeder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] is_out    true if the acceptor is an output acceptor.
 *  @param[in] temporary Temporary object used to store event queue.
 */
acceptor::acceptor(bool is_out, io::endpoint const* temporary)
  : io::endpoint(true),
    _is_out(is_out) {
  if (is_out && temporary)
    _temporary.reset(temporary->clone());
}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
acceptor::acceptor(acceptor const& right)
  : QObject(), io::endpoint(right), _is_out(right._is_out) {
  if (right._is_out && right._temporary.get())
    _temporary.reset(right._temporary->clone());
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  _from.clear();
  for (QList<QThread*>::iterator
         it(_threads.begin()),
         end(_threads.end());
       it != end;
       ++it)
    (*it)->wait();
}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& right) {
  if (this != &right) {
    io::endpoint::operator=(right);
    _is_out = right._is_out;
    if (right._is_out && right._temporary.get())
      _temporary.reset(right._temporary->clone());
  }
  return (*this);
}

/**
 *  Clone the acceptor.
 *
 *  @return This object.
 */
io::endpoint* acceptor::clone() const {
  std::auto_ptr<acceptor> copy(new acceptor(*this));
  if (_from.isNull())
    copy->_from = _from->clone();
  return (copy.release());
}

/**
 *  Close the acceptor.
 */
void acceptor::close() {
  if (!_from.isNull())
    _from->close();
  return ;
}

/**
 *  @brief Wait for incoming connection.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
misc::shared_ptr<io::stream> acceptor::open() {
  // Wait for client from the lower layer.
  if (!_from.isNull()) {
    misc::shared_ptr<io::stream> base(_from->open());

    if (!base.isNull()) {
      // In and out objects.
      misc::shared_ptr<io::stream> in;
      misc::shared_ptr<io::stream> out;

      // Create input and output objects.
      if (!_is_out) {
        in = misc::shared_ptr<io::stream>(new bbdo::input);
        in->read_from(base);
        in->write_to(base);
        out = misc::shared_ptr<io::stream>(new multiplexing::publisher);
      }
      else {
        in = misc::shared_ptr<io::stream>(
               new multiplexing::subscriber(_temporary.get()));
        out = misc::shared_ptr<io::stream>(new bbdo::output);
        out->read_from(base);
        out->write_to(base);
      }

      // Feeder thread.
      std::auto_ptr<processing::feeder> feedr(new processing::feeder);
      feedr->prepare(in, out);
      QObject::connect(
                 feedr.get(),
                 SIGNAL(finished()),
                 this,
                 SLOT(_on_thread_termination()));
      _threads.push_back(feedr.get());
      QObject::connect(
                 feedr.get(),
                 SIGNAL(finished()),
                 feedr.get(),
                 SLOT(deleteLater()));
      feedr.release()->start();
    }
  }

  return (misc::shared_ptr<io::stream>());
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Called when a thread terminates.
 */
void acceptor::_on_thread_termination() {
  QThread* th(static_cast<QThread*>(QObject::sender()));
  _threads.removeAll(th);
  return ;
}
