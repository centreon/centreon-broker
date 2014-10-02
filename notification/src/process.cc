/*
** Copyright 2009-2014 Merethis
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

#include <QStringList>
#include "com/centreon/broker/notification/process.hh"

using namespace com::centreon::broker::notification;

process::process()
  : _process(new QProcess),
    _running(false) {}


bool process::exec(std::string const& program,
                   std::list<std::string> const& arguments) {
  if (_running)
    return (false);
  _running = true;

  QStringList args;
  for (std::list<std::string>::const_iterator it(arguments.begin()),
                                              end(arguments.end());
       it != end; ++it)
    args << it->c_str();

  _process->start(program.c_str(), args);
  return (true);
}
