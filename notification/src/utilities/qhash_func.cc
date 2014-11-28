/*
** Copyright 2011-2013 Merethis
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

#include <QHash>
#include <QString>
#include <QByteArray>
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

/**
 *  QHash function for std::string.
 *
 *  @param str The std::string to hash.
 *  @return The hashed value for str.
 */
uint qHash(std::string const& str) {
  return (qHash(QByteArray::fromRawData(str.c_str(), str.size())));
}
