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

#include <algorithm>
#include <memory>
#include <QStringList>
#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
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
 *  @param[in] is_out     true if the acceptor is an output acceptor.
 *  @param[in] negociate  true if feature negociation is allowed.
 *  @param[in] extensions Available extensions.
 *  @param[in] temporary  Temporary object used to store event queue.
 */
acceptor::acceptor(
            bool is_out,
            bool negociate,
            QString const& extensions,
            io::endpoint const* temporary)
  : io::endpoint(true),
    _extensions(extensions),
    _is_out(is_out),
    _negociate(negociate) {
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
    _extensions = right._extensions;
    _is_out = right._is_out;
    _negociate = right._negociate;
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
      misc::shared_ptr<bbdo::stream> my_bbdo;
      if (!_is_out) {
        my_bbdo = misc::shared_ptr<bbdo::stream>(
                          new bbdo::stream(true, false));
        in = my_bbdo.staticCast<io::stream>();
        in->read_from(base);
        in->write_to(base);
        out = misc::shared_ptr<io::stream>(new multiplexing::publisher);
      }
      else {
        in = misc::shared_ptr<io::stream>(
                     new multiplexing::subscriber(_temporary.get()));
        my_bbdo = misc::shared_ptr<bbdo::stream>(
                          new bbdo::stream(false, true));
        out = my_bbdo.staticCast<io::stream>();
        out->read_from(base);
        out->write_to(base);
      }

      // Negociation.
      if (_negociate) {
        // Read initial packet.
        misc::shared_ptr<io::data> d;
        my_bbdo->read_any(d);
        if (d.isNull()
            || (d->type()
                != "com::centreon::broker::bbdo::version_response")) {
          logging::error(logging::high)
            << "BBDO: invalid protocol header, aborting connection";
          return (misc::shared_ptr<io::stream>());
        }

        // Handle protocol version.
        misc::shared_ptr<version_response>
          v(d.staticCast<version_response>());
        if (v->bbdo_major != BBDO_VERSION_MAJOR) {
          logging::error(logging::high)
            << "BBDO: peer is using protocol version " << v->bbdo_major
            << "." << v->bbdo_minor << "." << v->bbdo_patch
            << " whereas we're using protocol version "
            << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
            << BBDO_VERSION_PATCH;
          return (misc::shared_ptr<io::stream>());
        }
        logging::info(logging::medium)
          << "BBDO: peer is using protocol version " << v->bbdo_major
          << "." << v->bbdo_minor << "." << v->bbdo_patch
          << ", we're using version " << BBDO_VERSION_MAJOR << "."
          << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;

        // Send self version packet.
        misc::shared_ptr<version_response>
          welcome_packet(new version_response);
        welcome_packet->extensions = _extensions;
        my_bbdo->output::write(welcome_packet.staticCast<io::data>());
        my_bbdo->output::write(misc::shared_ptr<io::data>());

        // Apply negociated extensions.
        logging::info(logging::medium) << "BBDO: we have extensions '"
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
                                              base,
                                              true,
                                              *it));
                my_bbdo->read_from(s);
                my_bbdo->write_to(s);
                break ;
              }
          }
        }
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
