/*
** Copyright 2011 Merethis
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

#ifndef COMMON_HH_
# define COMMON_HH_

# include <QString>
# include <stddef.h>
# include "com/centreon/broker/logging/backend.hh"

// Log messages.
#define MSG1 "my first normal message"
#define MSG2 "my second foobar longer message"
#define MSG3 "my third message is even longer than the second"
#define MSG4 "my fourth messages is finally the longest of all bazqux"
#define MSG5 "my fifth message is shorter"
#define MSG6 "i'm tired of writing message, this is number 6"
#define MSG7 "my seventh message"
#define MSG8 "finally this is the last message for real"

bool    check_content(
          QString const& path,
          QString const& pattern,
          unsigned int msg_nb = 4,
          char const* const* lines = NULL);
QString temp_file_path(QString const& suffix = "");
void    write_log_messages(
          com::centreon::broker::logging::backend* b,
          unsigned int msg_nb = 4);

#endif /* !COMMON_HH_ */
