/*
** Copyright 2011-2012 Merethis
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
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/ndo/acceptor.hh"
#include "com/centreon/broker/ndo/input.hh"
#include "com/centreon/broker/ndo/output.hh"
#include "com/centreon/broker/processing/feeder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

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
  for (QVector<QThread*>::iterator
         it = _threads.begin(),
         end = _threads.end();
       it != end;
       ++it)
    if (th == *it) {
      _threads.erase(it);
      break ;
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] is_out    true if the acceptor is an output acceptor.
 *  @param[in] temporary XXX: todo.
 */
acceptor::acceptor(bool is_out, io::endpoint const* temporary)
  : io::endpoint(true),
    _is_out(is_out) {
  if (is_out && temporary)
    _temporary = std::auto_ptr<io::endpoint>(temporary->clone());
}

/**
 *  Copy constructor.
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : QObject(), io::endpoint(a) {
  _is_out = a._is_out;
  if (a._is_out && a._temporary.get())
    _temporary = std::auto_ptr<io::endpoint>(a._temporary->clone());
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  _from.clear();
  for (QVector<QThread*>::iterator
         it = _threads.begin(),
         end = _threads.end();
       it != end;
       ++it)
    (*it)->wait();
}

/**
 *  Assignment operator.
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  if (this != &a) {
    io::endpoint::operator=(a);
    _is_out = a._is_out;
    if (a._is_out && a._temporary.get())
      _temporary = std::auto_ptr<io::endpoint>(a._temporary->clone());
  }
  return (*this);
}

/**
 *  Clone the acceptor.
 *
 *  @return This object.
 */
io::endpoint* acceptor::clone() const {
  return (new acceptor(*this));
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
 *  Open the acceptor.
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
        in = misc::shared_ptr<io::stream>(new ndo::input);
        in->read_from(base);
        in->write_to(base);
        out = misc::shared_ptr<io::stream>(new multiplexing::publisher);
      }
      else {
        in = misc::shared_ptr<io::stream>(
               new multiplexing::subscriber(_temporary.get()));
        out = misc::shared_ptr<io::stream>(new ndo::output);
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
      processing::feeder* f(feedr.release());
      f->start();
    }
  }

  return (misc::shared_ptr<io::stream>());
}
