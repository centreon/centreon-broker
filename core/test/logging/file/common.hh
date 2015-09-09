/*
** Copyright 2011-2012 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef COMMON_HH
#  define COMMON_HH

#  include <cstddef>
#  include <QString>
#  include "com/centreon/broker/logging/backend.hh"

// Log messages.
#  define MSG1 "my first normal message"
#  define MSG2 "my second foobar longer message"
#  define MSG3 "my third message is even longer than the second"
#  define MSG4 "my fourth messages is finally the longest of all bazqux"
#  define MSG5 "my fifth message is shorter"
#  define MSG6 "i'm tired of writing message, this is number 6"
#  define MSG7 "my seventh message"
#  define MSG8 "finally this is the last message for real"

bool    check_content(
          QString const& path,
          QString const& pattern,
          unsigned int msg_nb = 4,
          char const* const* lines = NULL);
QString temp_file_path();
void    write_log_messages(
          com::centreon::broker::logging::backend* b,
          unsigned int msg_nb = 4);

#endif // !COMMON_HH
