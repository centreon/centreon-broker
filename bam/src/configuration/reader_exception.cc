/*
** Copyright 2014 Merethis
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

#include  "com/centreon/broker/bam/configuration/reader_exception.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor
 */
reader_exception::reader_exception() : msg() {}

/**
 *  Copy constructor
 *
 *  @param[in] Other object
 */
reader_exception(reader_exception const& other) : msg(other) {}

/**
 *  Destructor
 */
reader_exception::~reader_exception() {}

/**
 *  Assignment operator
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
reader_exception& reader_exception::operator=(
                                      reader_exception const& other) {
  if (this != &other) {
    msg = other;
  }
  return (*this);
}
