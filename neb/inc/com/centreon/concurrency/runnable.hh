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

#ifndef CC_CONCURRENCY_RUNNABLE_HH
#define CC_CONCURRENCY_RUNNABLE_HH

#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
/**
 *  @class runnable runnable.hh "com/centreon/concurrency/runnable.hh"
 *  @brief Base for all runnable objects.
 *
 *  This class is an interface for piece of code to needs to be
 *  executed.
 */
class runnable {
 public:
  runnable();
  runnable(runnable const& right);
  virtual ~runnable() throw();
  runnable& operator=(runnable const& right);
  bool get_auto_delete() const throw();
  virtual void run() = 0;
  void set_auto_delete(bool auto_delete) throw();

 private:
  runnable& _internal_copy(runnable const& right);

  bool _auto_delete;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_RUNNABLE_HH
