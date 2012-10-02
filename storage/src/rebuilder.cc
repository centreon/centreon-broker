/*
** Copyright 2012 Merethis
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

#include "com/centreon/broker/storage/rebuilder.hh"

using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
rebuilder::rebuilder() : _should_exit(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
rebuilder::rebuilder(rebuilder const& right) : QThread() {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
rebuilder::~rebuilder() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
rebuilder& rebuilder::operator=(rebuilder const& right) {
  if (this != &right) {
    if (_db.isOpen())
      _db.close();
    if (!_db.connectionName().isEmpty())
      QSqlDatabase::removeDatabase(_db.connectionName());
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Set the exit flag.
 */
void rebuilder::exit() throw () {
  _should_exit = true;
  return ;
}

/**
 *  Get the rebuild check interval.
 *
 *  @return Rebuild check interval in seconds.
 */
unsigned int rebuilder::get_interval() const throw () {
  return (_interval);
}

/**
 *  Thread entry point.
 */
void rebuilder::run() {
}

/**
 *  Set the database object.
 *
 *  @param[in] db DB object to copy.
 */
void rebuilder::set_db(QSqlDatabase const& db) {
  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Remove old DB.
  QSqlDatabase::removeDatabase(id);

  // Clone database.
  _db = QSqlDatabase::cloneDatabase(db, id);

  return ;
}

/**
 *  Set the rebuild check interval.
 *
 *  @param[in] interval Rebuild check interval in seconds.
 */
void rebuilder::set_interval(unsigned int interval) throw () {
  _interval = interval;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void rebuilder::_internal_copy(rebuilder const& right) {
  // Copy DB.
  QString id;
  id.setNum((qulonglong)this, 16);
  _db = QSqlDatabase::cloneDatabase(right._db, id);

  // Copy interval.
  _interval = right._interval;

  return ;
}
