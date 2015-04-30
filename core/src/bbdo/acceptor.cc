/*
** Copyright 2013-2015 Merethis
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

#include <algorithm>
#include <memory>
#include <QStringList>
#include <sstream>
#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
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
 *  @param[in] name                    The name to build temporary.
 *  @param[in] is_out                  true if the acceptor is an output
 *                                     acceptor.
 *  @param[in] negociate               true if feature negociation is
 *                                     allowed.
 *  @param[in] extensions              Available extensions.
 *  @param[in] timeout                 Connection timeout.
 *  @param[in] one_peer_retention_mode True to enable the "one peer
 *                                     retention mode" (TM).
 *  @param[in] coarse                  If the acceptor is coarse or not.
 */
acceptor::acceptor(
            QString const& name,
            bool is_out,
            bool negociate,
            QString const& extensions,
            time_t timeout,
            bool one_peer_retention_mode,
            bool coarse)
  : io::endpoint(!one_peer_retention_mode),
    _coarse(coarse),
    _extensions(extensions),
    _is_out(is_out),
    _name(name),
    _negociate(negociate),
    _one_peer_retention_mode(one_peer_retention_mode),
    _timeout(timeout) {
  if ((_timeout == (time_t)-1) || (_timeout == 0))
    _timeout = 3;
}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
acceptor::acceptor(acceptor const& right)
  : QObject(),
    io::endpoint(right),
    _coarse(right._coarse),
    _extensions(right._extensions),
    _is_out(right._is_out),
    _name(right._name),
    _negociate(right._negociate),
    _one_peer_retention_mode(right._one_peer_retention_mode),
    _timeout(right._timeout) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  _from.clear();
  for (QList<QThread*>::iterator
         it(_clients.begin()),
         end(_clients.end());
       it != end;
       ++it) {
    (*it)->wait();
    delete *it;
  }
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
    _coarse = right._coarse;
    _extensions = right._extensions;
    _is_out = right._is_out;
    _name = right._name;
    _negociate = right._negociate;
    _one_peer_retention_mode = right._one_peer_retention_mode;
    _timeout = right._timeout;
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
 *  Wait for incoming connection.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
misc::shared_ptr<io::stream> acceptor::open() {
  // Clean client threads.
  for (QList<QThread*>::iterator
         it(_clients.begin()),
         end(_clients.end());
       it != end;) {
    if ((*it)->wait(0)) {
      QList<QThread*>::iterator to_delete(it);
      ++it;
      delete *to_delete;
      _clients.erase(to_delete);
    }
    else
      ++it;
  }

  // Wait for client from the lower layer.
  if (!_from.isNull()) {
    if (_one_peer_retention_mode) {
      misc::shared_ptr<io::stream> s(_from->open());
      if (!s.isNull()) {
        misc::shared_ptr<bbdo::stream>
          my_bbdo(_is_out
                  ? new bbdo::stream(false, true)
                  : new bbdo::stream(true, false));
        my_bbdo->read_from(s);
        my_bbdo->write_to(s);
        _negociate_features(s, my_bbdo);
        return (my_bbdo);
      }
    }
    else {
      misc::shared_ptr<io::stream> s(_open(_from->open()));
      if (!s.isNull()) {
        _clients.push_back(new helper(this, s));
        _clients.last()->start();
      }
    }
  }

  return (misc::shared_ptr<io::stream>());
}

/**
 *  Wait for incoming connection.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
misc::shared_ptr<io::stream> acceptor::open(QString const& id) {
  // Clean client threads.
  for (QList<QThread*>::iterator
         it(_clients.begin()),
         end(_clients.end());
       it != end;) {
    if ((*it)->wait(0)) {
      QList<QThread*>::iterator to_delete(it);
      ++it;
      delete *to_delete;
      _clients.erase(to_delete);
    }
    else
      ++it;
  }

  // Wait for client from the lower layer.
  if (!_from.isNull()) {
    if (_one_peer_retention_mode) {
      misc::shared_ptr<io::stream> s(_from->open(id));
      if (!s.isNull()) {
        misc::shared_ptr<bbdo::stream>
          my_bbdo(_is_out
                  ? new bbdo::stream(false, true)
                  : new bbdo::stream(true, false));
        my_bbdo->read_from(s);
        my_bbdo->write_to(s);
        _negociate_features(s, my_bbdo);
        return (my_bbdo);
      }
    }
    else {
      misc::shared_ptr<io::stream> s(_open(_from->open(id)));
      if (!s.isNull()) {
        _clients.push_back(new helper(this, s));
        _clients.last()->start();
      }
    }
  }

  return (misc::shared_ptr<io::stream>());
}

/**
 *  Get BBDO statistics.
 *
 *  @param[out] tree Properties tree.
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
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Negociate stream features.
 *
 *  @param[in]     stream   Base stream (no BBDO).
 *  @param[in,out] my_bbdo  BBDO stream being processed.
 *
 *  @return Instance ID of the negociated stream.
 */
unsigned int acceptor::_negociate_features(
                         misc::shared_ptr<io::stream> stream,
                         misc::shared_ptr<bbdo::stream> my_bbdo) {
  // Coarse peer don't expect any salutation either.
  if (_coarse)
    return (0);

  // Read initial packet.
  misc::shared_ptr<io::data> d;
  my_bbdo->read_any(d, time(NULL) + _timeout);
  if (d.isNull() || (d->type() != version_response::static_type()))
    throw (exceptions::msg()
           << "BBDO: invalid protocol header, aborting connection");

  // Handle protocol version.
  misc::shared_ptr<version_response>
    v(d.staticCast<version_response>());
  if (v->bbdo_major != BBDO_VERSION_MAJOR)
    throw (exceptions::msg()
           << "BBDO: peer is using protocol version " << v->bbdo_major
           << "." << v->bbdo_minor << "." << v->bbdo_patch
           << " whereas we're using protocol version "
           << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
           << BBDO_VERSION_PATCH);
  logging::info(logging::medium)
    << "BBDO: peer is using protocol version " << v->bbdo_major
    << "." << v->bbdo_minor << "." << v->bbdo_patch
    << ", we're using version " << BBDO_VERSION_MAJOR << "."
    << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;

  // Send self version packet.
  misc::shared_ptr<version_response>
    welcome_packet(new version_response);
  if (_negociate)
    welcome_packet->extensions = _extensions;
  my_bbdo->output::write(welcome_packet);
  my_bbdo->output::write(misc::shared_ptr<io::data>());

  // Negociation.
  if (_negociate) {
    // Apply negociated extensions.
    logging::info(logging::medium)
      << "BBDO: we have extensions '"
      << _extensions << "' and peer has '" << v->extensions << "'";
    QStringList own_ext(_extensions.split(' '));
    QStringList peer_ext(v->extensions.split(' '));
    for (QStringList::const_iterator
           it(own_ext.begin()),
           end(own_ext.end());
         it != end;
         ++it) {
      // Find matching extension in peer extension list.
      QStringList::const_iterator
        peer_it(std::find(peer_ext.begin(), peer_ext.end(), *it));
      // Apply extension if found.
      if (peer_it != peer_ext.end()) {
        logging::info(logging::medium)
          << "BBDO: applying extension '" << *it << "'";
        for (QMap<QString, io::protocols::protocol>::const_iterator
               proto_it(io::protocols::instance().begin()),
               proto_end(io::protocols::instance().end());
             proto_it != proto_end;
             ++proto_it)
          if (proto_it.key() == *it) {
            misc::shared_ptr<io::stream>
              s(proto_it->endpntfactry->new_stream(
                                          stream,
                                          true,
                                          *it));
              my_bbdo->read_from(s);
              my_bbdo->write_to(s);
              break ;
          }
      }
    }
  }
  return (d->source_id);
}

/**
 *  Called when a thread terminates.
 */
void acceptor::_on_thread_termination() {
  QThread* th(static_cast<QThread*>(QObject::sender()));
  QMutexLocker lock(&_threadsm);
  _threads.removeAll(th);
  return ;
}

/**
 *  Wait for incoming connection.
 *
 *  @param[in] stream  Lower-layer stream.
 *
 *  @return Always return null stream. A new thread will be launched to
 *          process the incoming connection.
 */
misc::shared_ptr<io::stream> acceptor::_open(
                                          misc::shared_ptr<io::stream> stream) {
  if (!stream.isNull()) {
    // In and out objects.
    misc::shared_ptr<io::stream> in;
    misc::shared_ptr<io::stream> out;

    // Create input and output objects.
    misc::shared_ptr<bbdo::stream> my_bbdo;
    if (!_is_out) {
      my_bbdo = misc::shared_ptr<bbdo::stream>(
                        new bbdo::stream(true, false));
      in = my_bbdo;
      in->read_from(stream);
      in->write_to(stream);
      out = misc::shared_ptr<io::stream>(new multiplexing::publisher);
    }
    else {
      misc::shared_ptr<multiplexing::subscriber> sbcr(
        new multiplexing::subscriber(_name));
      sbcr->set_filters(_filter);
      logging::error(logging::high)
        << "TEST: setting multiplexing filter for " <<_filter.size();
      in = sbcr;
      my_bbdo = misc::shared_ptr<bbdo::stream>(
                        new bbdo::stream(false, true));
      out = my_bbdo;
      out->read_from(stream);
      out->write_to(stream);
    }

    // Negociate features.
    unsigned int instance_id;
    try {
      instance_id = _negociate_features(stream, my_bbdo);
    }
    catch (exceptions::msg const& e) {
      logging::info(logging::high) << e.what();
      return (misc::shared_ptr<io::stream>());
    }

    // Feeder thread.
    std::auto_ptr<processing::feeder> feedr(new processing::feeder);
    std::ostringstream oss;
    oss << "instance #" << instance_id;
    feedr->prepare(oss.str(), in, out);
    QObject::connect(
               feedr.get(),
               SIGNAL(finished()),
               this,
               SLOT(_on_thread_termination()));
    {
      QMutexLocker lock(&_threadsm);
      _threads.push_back(feedr.get());
    }
    QObject::connect(
               feedr.get(),
               SIGNAL(finished()),
               feedr.get(),
               SLOT(deleteLater()));
    feedr.release()->start();
  }

  return (misc::shared_ptr<io::stream>());
}

/**************************************
*                                     *
*           Helper Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] accptr Acceptor object.
 *  @param[in] s      Stream object.
 */
acceptor::helper::helper(
                    acceptor* accptr,
                    misc::shared_ptr<io::stream> s)
  : _acceptor(accptr), _stream(s) {}

/**
 *  Thread entry point.
 */
void acceptor::helper::run() {
  try {
    _acceptor->_open(_stream);
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "BBDO: client handshake failed: " << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "BBDO: client handshake failed: unknown error";
  }
  return ;
}
