/*
** Copyright 2011-2013 Centreon
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

#ifndef PARSER_COMMON_HH
#define PARSER_COMMON_HH

#include <QMap>
#include <QPair>
#include "com/centreon/broker/correlation/node.hh"

void compare_states(QMap<QPair<unsigned int, unsigned int>,
                         com::centreon::broker::correlation::node> const& n1,
                    QMap<QPair<unsigned int, unsigned int>,
                         com::centreon::broker::correlation::node> const& n2);

#endif  // !PARSER_COMMON_HH
