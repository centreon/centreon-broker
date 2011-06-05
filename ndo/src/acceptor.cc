/*
** Copyright 2011 Merethis
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

#include <QSharedPointer>
#include "multiplexing/publisher.hh"
#include "multiplexing/subscriber.hh"
#include "ndo/acceptor.hh"
#include "ndo/input.hh"
#include "ndo/output.hh"
#include "processing/feeder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] is_out true if the acceptor is an output acceptor.
 */
acceptor::acceptor(bool is_out) : _is_out(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : io::acceptor(a) {
  _is_out = a._is_out;
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {}

/**
 *  Assignment operator.
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  io::acceptor::operator=(a);
  _is_out = a._is_out;
  return (*this);
}

/**
 *  Accept an incoming connection.
 *
 *  @param[in] ptr New connection object.
 */
void acceptor::accept(QSharedPointer<io::stream> ptr) {
  // In and out objects.
  QSharedPointer<io::stream> in;
  QSharedPointer<io::stream> out;

  // Create input and output objects.
  if (!_is_out) {
    in = QSharedPointer<io::stream>(new ndo::input);
    in->read_from(ptr);
    out = QSharedPointer<io::stream>(new multiplexing::publisher);
  }
  else {
    in = QSharedPointer<io::stream>(new multiplexing::subscriber);
    out = QSharedPointer<io::stream>(new ndo::output);
    out->write_to(ptr);
  }

  // Feeder thread.
  QScopedPointer<processing::feeder> feedr(new processing::feeder);
  feedr->prepare(in, out);
  QObject::connect(feedr.data(), SIGNAL(finished()), feedr.data(), SLOT(deleteLater()));
  processing::feeder* f(feedr.take());
  f->run();

  return ;
}

/**
 *  Close the acceptor.
 */
void acceptor::close() {
  return ;
}

/**
 *  Open the acceptor.
 */
QSharedPointer<io::stream> acceptor::open() {
  QSharedPointer<io::stream> retval;
  if (!_down.isNull()) {
    retval = _down->open();
    QSharedPointer<io::stream> ndo_stream;
    ndo_stream = QSharedPointer<io::stream>(new ndo::input);
    ndo_stream->read_from(retval);
    ndo_stream->write_to(retval);
    retval = ndo_stream;
  }
  return (retval);
}
