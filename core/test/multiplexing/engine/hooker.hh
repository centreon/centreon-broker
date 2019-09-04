/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#ifndef HOOKER_HH
#define HOOKER_HH

#include <queue>
#include "com/centreon/broker/multiplexing/hooker.hh"

#define HOOKMSG1 "my first hooking message (when engine is started)"
#define HOOKMSG2 "my second hooking message (when multiplexing events)"
#define HOOKMSG3 "my third hooking message (when engine is stopped)"

using namespace com::centreon::broker;

/**
 *  @class hooker hooker.hh "test/multiplexing/engine/hooker.hh"
 *  @brief Test hook class.
 *
 *  Simple class that hook events from the multiplexing engine.
 */
class hooker : public multiplexing::hooker {
 public:
  hooker();
  ~hooker();
  hooker(hooker const& other) = delete;
  hooker& operator=(hooker const& other) = delete;
  bool read(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  void starting();
  void stopping();
  int write(std::shared_ptr<io::data> const& d);

 private:
  std::queue<std::shared_ptr<io::data> > _queue;
};

#endif  // !HOOKER_HH
