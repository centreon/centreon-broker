/*
** Copyright 2011-2013,2015 Merethis
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
 *  @param[in] name                    The name to build temporary.
 *  @param[in] is_out                  True if the acceptor is an output
 *                                     acceptor.
 *  @param[in] one_peer_retention_mode True to enable the "one peer
 *                                     retention mode" (TM).
 */
acceptor::acceptor(
            QString const& name,
            bool is_out,
            bool one_peer_retention_mode)
  : io::endpoint(!one_peer_retention_mode),
    _is_out(is_out),
    _name(name),
    _one_peer_retention_mode(one_peer_retention_mode) {}

/**
 *  Copy constructor.
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : QObject(), io::endpoint(a) {
  _is_out = a._is_out;
  _name = a._name;
  _one_peer_retention_mode = a._one_peer_retention_mode;
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
    _name = a._name;
    _one_peer_retention_mode = a._one_peer_retention_mode;
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
  if (!_from.isNull())
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
 *  Open the acceptor.
 *
 *  @param[in] cache  NDO module does not use the persistent cache.
 */
misc::shared_ptr<io::stream> acceptor::open(persistent_cache* cache) {
  (void)cache;

  // Wait for client from the lower layer.
  if (!_from.isNull()) {
    if (_one_peer_retention_mode) {
      misc::shared_ptr<io::stream> sub(_from->open());
      if (!sub.isNull()) {
        misc::shared_ptr<io::stream>
          ours(_is_out
               ? static_cast<io::stream*>(new ndo::output)
               : static_cast<io::stream*>(new ndo::input));
        ours->read_from(sub);
        ours->write_to(sub);
        return (ours);
      }
    }
    else
      _open(_from->open());
  }
  return (misc::shared_ptr<io::stream>());
}

/**
 *  Open the acceptor.
 *
 *  @param[in] id     Connection ID.
 *  @param[in] cache  NDO module does not use the persistent cache.
 */
misc::shared_ptr<io::stream> acceptor::open(
                                         QString const& id,
                                         persistent_cache* cache) {
  (void)cache;

  // Wait for client from the lower layer.
  if (!_from.isNull()) {
    if (_one_peer_retention_mode) {
      misc::shared_ptr<io::stream> sub(_from->open(id));
      if (!sub.isNull()) {
        misc::shared_ptr<io::stream>
          ours(_is_out
               ? static_cast<io::stream*>(new ndo::output)
               : static_cast<io::stream*>(new ndo::input));
        ours->read_from(sub);
        ours->write_to(sub);
        return (ours);
      }
    }
    else
      _open(_from->open(id));
  }
  return (misc::shared_ptr<io::stream>());
}

/**
 *  Get NDO statistics.
 *
 *  @param[out] buffer Buffer.
 */
void acceptor::stats(io::properties& tree) {
  io::property& p(tree["one_peer_retention_mode"]);
  p.set_perfdata(_one_peer_retention_mode
                 ? "one peer retention mode=true"
                 : "one peer retention mode=false");
  p.set_graphable(false);
  if (!_from.isNull())
    _from->stats(tree);
  return ;
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Open the acceptor.
 */
misc::shared_ptr<io::stream> acceptor::_open(
  misc::shared_ptr<io::stream> stream) {
  if (!stream.isNull()) {
    // In and out objects.
    misc::shared_ptr<io::stream> in;
    misc::shared_ptr<io::stream> out;

    // Create input and output objects.
    if (!_is_out) {
      in = misc::shared_ptr<io::stream>(new ndo::input);
      in->read_from(stream);
      in->write_to(stream);
      out = misc::shared_ptr<io::stream>(new multiplexing::publisher);
    }
    else {
      in = misc::shared_ptr<io::stream>(
                   new multiplexing::subscriber(_name));
      out = misc::shared_ptr<io::stream>(new ndo::output);
      out->read_from(stream);
      out->write_to(stream);
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

  return (misc::shared_ptr<io::stream>());
}
