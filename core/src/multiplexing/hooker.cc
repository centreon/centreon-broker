/*
** Copyright 2011 Centreon
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

#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/hooker.hh"

using namespace com::centreon::broker::multiplexing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
hooker::hooker() : _registered(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] h Object to copy.
 */
hooker::hooker(hooker const& h) : QObject(), io::stream(h) {}

/**
 *  Destructor.
 */
hooker::~hooker() {}

/**
 *  Assignment operator.
 *
 *  @param[in] h Object to copy.
 *
 *  @return This object.
 */
hooker& hooker::operator=(hooker const& h) {
  io::stream::operator=(h);
  return (*this);
}

/**
 *  Enable or disable hooking.
 *
 *  @param[in] in  Set to false will disable hooking.
 *  @param[in] out Set to false will isable hooking.
 */
void hooker::process(bool in, bool out) {
  if (_registered && (!in || !out)) {
    engine::instance().unhook(*this);
    _registered = false;
  }
  else if (!_registered && (in || out)) {
    engine::instance().hook(*this);
    _registered = true;
  }
  return ;
}
