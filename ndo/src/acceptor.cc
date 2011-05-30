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
#include "multiplexing/subscriber.hh"
#include "ndo/acceptor.hh"
#include "ndo/input.hh"
#include "processing/feeder.hh"
#include "serialization/iserial.hh"
#include "serialization/oserial.hh"

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
  if (!_is_out) {
    // Create input and objects.
    QSharedPointer<serialization::iserial> in(new ndo::input);
    in->read_from(ptr);
    QSharedPointer<serialization::oserial> out(new multiplexing::subscriber);

    // Feeder thread.
    QScopedPointer<processing::feeder> feedr(new processing::feeder);
    feedr->prepare(in, out);
    QObject::connect(feedr.data(), SIGNAL(finished()), feedr.data(), SLOT(deleteLater()));
    processing::feeder* f(feedr.take());
    f->run();
  }
  else {
    // XXX
  }
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
void acceptor::open() {
  return ;
}
