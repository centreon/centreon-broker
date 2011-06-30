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

#ifndef COMMON_HH_
# define COMMON_HH_

# include <QString>
# include "com/centreon/broker/logging/backend.hh"

bool    check_content(QString const& path, QString const& pattern);
QString temp_file_path();
void    write_log_messages(com::centreon::broker::logging::backend* b);

#endif /* !COMMON_HH_ */
